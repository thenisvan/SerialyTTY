#include "sd_logger.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <time.h>

static const char *TAG = "SD_LOGGER";

// SD Card SPI pins for ESP32-C6
#define SD_MOSI_PIN     GPIO_NUM_7
#define SD_MISO_PIN     GPIO_NUM_2
#define SD_SCLK_PIN     GPIO_NUM_6
#define SD_CS_PIN_SPI   GPIO_NUM_10  // Renamed to avoid conflict

#define MOUNT_POINT     "/sdcard"
#define LOG_DIR         MOUNT_POINT "/logs"

SDLogger::SDLogger() : sdInitialized(false), logCounter(0), fileHandle(nullptr) {
    currentLogFile[0] = '\0';
}

SDLogger::~SDLogger() {
    close();
}

bool SDLogger::begin() {
    ESP_LOGI(TAG, "Initializing SD card...");
    
    // Configure SPI bus for SD card
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SD_MOSI_PIN,
        .miso_io_num = SD_MISO_PIN,
        .sclk_io_num = SD_SCLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };
    
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return false;
    }
    
    // Mount FAT filesystem
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    
    sdmmc_card_t* card;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_DEFAULT;
    
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SD_CS_PIN_SPI;
    slot_config.host_id = SPI2_HOST;
    
    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGW(TAG, "Failed to mount filesystem. SD card may not be present.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SD card: %s", esp_err_to_name(ret));
        }
        return false;
    }
    
    // Print card info
    ESP_LOGI(TAG, "SD card mounted successfully");
    ESP_LOGI(TAG, "Card size: %lluMB", ((uint64_t)card->csd.capacity) * card->csd.sector_size / (1024 * 1024));
    
    // Create logs directory
    struct stat st;
    if (stat(LOG_DIR, &st) != 0) {
        if (mkdir(LOG_DIR, 0775) != 0) {
            ESP_LOGE(TAG, "Failed to create logs directory");
            esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
            return false;
        }
        ESP_LOGI(TAG, "Created logs directory");
    }
    
    sdInitialized = true;
    openLogFile();
    
    // Log initialization
    writeLog("INFO", "SD Logger initialized");
    
    return true;
}

void SDLogger::openLogFile() {
    if (!sdInitialized) return;
    
    // Generate filename with timestamp
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    
    snprintf(currentLogFile, sizeof(currentLogFile), 
             "%s/log_%04d%02d%02d_%02d%02d%02d.txt",
             LOG_DIR,
             timeinfo.tm_year + 1900,
             timeinfo.tm_mon + 1,
             timeinfo.tm_mday,
             timeinfo.tm_hour,
             timeinfo.tm_min,
             timeinfo.tm_sec);
    
    // Open file in append mode
    FILE* f = fopen(currentLogFile, "a");
    if (f == nullptr) {
        ESP_LOGE(TAG, "Failed to open log file: %s", currentLogFile);
        sdInitialized = false;
        return;
    }
    
    fileHandle = (void*)f;
    ESP_LOGI(TAG, "Logging to: %s", currentLogFile);
}

void SDLogger::getTimestamp(char* buffer, size_t size) {
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    
    snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d",
             timeinfo.tm_year + 1900,
             timeinfo.tm_mon + 1,
             timeinfo.tm_mday,
             timeinfo.tm_hour,
             timeinfo.tm_min,
             timeinfo.tm_sec);
}

void SDLogger::writeLog(const char* level, const char* message) {
    if (!sdInitialized || fileHandle == nullptr) {
        ESP_LOGI(TAG, "[%s] %s", level, message);
        return;
    }
    
    char timestamp[32];
    getTimestamp(timestamp, sizeof(timestamp));
    
    FILE* f = (FILE*)fileHandle;
    fprintf(f, "[%s] [%s] %s\n", timestamp, level, message);
    
    logCounter++;
    
    // Auto-flush every 10 logs
    if (logCounter % 10 == 0) {
        fflush(f);
    }
}

void SDLogger::log(const char* message) {
    writeLog("INFO", message);
}

void SDLogger::logData(const char* direction, const uint8_t* data, size_t len) {
    if (!sdInitialized) {
        ESP_LOGI(TAG, "[DATA-%s] %zu bytes", direction, len);
        return;
    }
    
    char buffer[256];
    int offset = snprintf(buffer, sizeof(buffer), "DATA-%s: ", direction);
    
    // Convert data to hex string (limit to 64 bytes for readability)
    size_t displayLen = (len > 64) ? 64 : len;
    for (size_t i = 0; i < displayLen && offset < (int)sizeof(buffer) - 4; i++) {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%02X ", data[i]);
    }
    
    if (len > 64) {
        snprintf(buffer + offset, sizeof(buffer) - offset, "... (%zu total)", len);
    }
    
    writeLog("DATA", buffer);
}

void SDLogger::logBaudDetection(uint32_t baud) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Baud rate detected: %lu bps", (unsigned long)baud);
    writeLog("BAUD", buffer);
}

void SDLogger::logStateChange(SystemState state) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "State changed to: %s", getStateName(state));
    writeLog("STATE", buffer);
}

void SDLogger::logTestResult(bool success) {
    writeLog("TEST", success ? "Test PASSED" : "Test FAILED");
}

void SDLogger::flush() {
    if (!sdInitialized || fileHandle == nullptr) return;
    
    FILE* f = (FILE*)fileHandle;
    fflush(f);
    
    // Sync to physical media
    // Note: fsync not available in newlib, fflush is sufficient
}

void SDLogger::close() {
    if (!sdInitialized) return;
    
    if (fileHandle != nullptr) {
        FILE* f = (FILE*)fileHandle;
        writeLog("INFO", "SD Logger closing");
        fflush(f);
        fclose(f);
        fileHandle = nullptr;
    }
    
    // Unmount SD card
    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, nullptr);
    sdInitialized = false;
    
    ESP_LOGI(TAG, "SD card unmounted");
}

void SDLogger::ensureSDReady() {
    if (!sdInitialized) {
        ESP_LOGW(TAG, "SD card not initialized");
    }
}

const char* SDLogger::getStateName(SystemState state) {
    switch(state) {
        case STATE_BOOTING: return "BOOTING";
        case STATE_MENU: return "MENU";
        case STATE_ANALYZING: return "ANALYZING";
        case STATE_BRIDGE_MODE: return "BRIDGE_MODE";
        case STATE_WAITING: return "WAITING";
        case STATE_FOUND_SPEED: return "FOUND_SPEED";
        case STATE_RESTART_NEEDED: return "RESTART_NEEDED";
        case STATE_TESTING: return "TESTING";
        case STATE_RUNNING: return "RUNNING";
        default: return "UNKNOWN";
    }
}
