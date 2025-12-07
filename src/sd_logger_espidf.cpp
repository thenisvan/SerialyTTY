#include "sd_logger.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "SD_LOGGER";

SDLogger::SDLogger() {
    sdInitialized = false;
    logCounter = 0;
}

bool SDLogger::begin() {
    // TODO: Initialize SD card with ESP-IDF SD/MMC driver
    ESP_LOGW(TAG, "SD card not yet implemented for ESP-IDF");
    sdInitialized = false;
    return false;
}

void SDLogger::log(String message) {
    if (!sdInitialized) {
        ESP_LOGI(TAG, "[LOG] %s", message.c_str());
        return;
    }
    
    // TODO: Write to SD card file
    logCounter++;
}

void SDLogger::logData(String direction, String data) {
    if (!sdInitialized) {
        ESP_LOGI(TAG, "[DATA-%s] %s", direction.c_str(), data.c_str());
        return;
    }
    
    // TODO: Write to SD card file
    logCounter++;
}

void SDLogger::logBaudDetection(uint32_t baud) {
    if (!sdInitialized) {
        ESP_LOGI(TAG, "[BAUD] Detected: %lu bps", (unsigned long)baud);
        return;
    }
    
    // TODO: Write to SD card file
    logCounter++;
}

void SDLogger::logStateChange(SystemState state) {
    const char* stateName = "Unknown";
    switch(state) {
        case STATE_BOOTING: stateName = "BOOTING"; break;
        case STATE_WAITING: stateName = "WAITING"; break;
        case STATE_ANALYZING: stateName = "ANALYZING"; break;
        case STATE_FOUND_SPEED: stateName = "FOUND_SPEED"; break;
        case STATE_RESTART_NEEDED: stateName = "RESTART_NEEDED"; break;
        case STATE_TESTING: stateName = "TESTING"; break;
        case STATE_RUNNING: stateName = "RUNNING"; break;
    }
    
    if (!sdInitialized) {
        ESP_LOGI(TAG, "[STATE] Changed to: %s", stateName);
        return;
    }
    
    // TODO: Write to SD card file
    logCounter++;
}

void SDLogger::logTestResult(bool success) {
    if (!sdInitialized) {
        ESP_LOGI(TAG, "[TEST] Result: %s", success ? "SUCCESS" : "FAILED");
        return;
    }
    
    // TODO: Write to SD card file
    logCounter++;
}

void SDLogger::flush() {
    if (!sdInitialized) return;
    // TODO: Flush SD card file
}

void SDLogger::close() {
    if (!sdInitialized) return;
    // TODO: Close SD card file
    sdInitialized = false;
}

const char* SDLogger::getFileName() {
    // Return a fixed string; avoid dynamic String for C-string return
    static const char filename[] = "/sdcard/sniffer.log";
    return filename;
}

void SDLogger::ensureSDReady() {
    // TODO: Check SD card status
}
