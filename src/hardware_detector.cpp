#include "hardware_detector.h"
#include "config.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include <cstring>

static const char *TAG = "HW_DETECT";

// Common I2C addresses for display controllers
#define ILI9341_I2C_ADDR    0x3C  // Some ILI9341 with I2C interface
#define SSD1306_I2C_ADDR    0x3C  // OLED display
#define SSD1306_ALT_ADDR    0x3D  // Alternative OLED address
#define ST7735_I2C_ADDR     0x3C  // Some ST7735 displays

// Accelerometer addresses
#define MPU6050_I2C_ADDR    0x68
#define ADXL345_I2C_ADDR    0x53

HardwareDetector::HardwareDetector() : 
    i2cInitialized(false)
{
    memset(&config, 0, sizeof(config));
}

void HardwareDetector::initI2C() {
    if (i2cInitialized) return;
    
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_PIN;
    conf.scl_io_num = I2C_SCL_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;  // 100kHz for compatibility
    conf.clk_flags = 0;
    
    esp_err_t ret = i2c_param_config(I2C_NUM_0, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config failed: %s", esp_err_to_name(ret));
        return;
    }
    
    ret = i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed: %s", esp_err_to_name(ret));
        return;
    }
    
    i2cInitialized = true;
    ESP_LOGI(TAG, "I2C initialized on SDA=%d, SCL=%d", I2C_SDA_PIN, I2C_SCL_PIN);
}

void HardwareDetector::deinitI2C() {
    if (!i2cInitialized) return;
    
    i2c_driver_delete(I2C_NUM_0);
    i2cInitialized = false;
    ESP_LOGD(TAG, "I2C deinitialized");
}

bool HardwareDetector::checkI2CDevice(uint8_t address) {
    if (!i2cInitialized) {
        initI2C();
    }
    
    if (!i2cInitialized) {
        return false;
    }
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);
    
    return (ret == ESP_OK);
}

bool HardwareDetector::scanI2CBus(uint8_t* foundDevices, size_t maxDevices) {
    if (!i2cInitialized) {
        initI2C();
    }
    
    if (!i2cInitialized) {
        return false;
    }
    
    size_t deviceCount = 0;
    ESP_LOGI(TAG, "Scanning I2C bus...");
    
    for (uint8_t addr = 1; addr < 127 && deviceCount < maxDevices; addr++) {
        if (checkI2CDevice(addr)) {
            ESP_LOGI(TAG, "  Found I2C device at 0x%02X", addr);
            if (foundDevices) {
                foundDevices[deviceCount] = addr;
            }
            deviceCount++;
        }
    }
    
    ESP_LOGI(TAG, "I2C scan complete: %d device(s) found", deviceCount);
    return (deviceCount > 0);
}

bool HardwareDetector::detectDisplay() {
    ESP_LOGI(TAG, "Detecting display...");
    
    // Check common display I2C addresses
    uint8_t displayAddresses[] = {
        ILI9341_I2C_ADDR,
        SSD1306_I2C_ADDR,
        SSD1306_ALT_ADDR,
        ST7735_I2C_ADDR
    };
    
    for (size_t i = 0; i < sizeof(displayAddresses); i++) {
        if (checkI2CDevice(displayAddresses[i])) {
            config.displayPresent = true;
            config.displayAddress = displayAddresses[i];
            ESP_LOGI(TAG, "Display detected at I2C address 0x%02X", displayAddresses[i]);
            return true;
        }
    }
    
    // For SPI displays like ILI9341, we can't easily detect via I2C
    // Instead, check if the CS pin shows expected behavior or use a test write
    // For now, we'll assume no display if no I2C device found
    
    config.displayPresent = false;
    config.displayAddress = 0;
    ESP_LOGI(TAG, "No I2C display detected");
    return false;
}

bool HardwareDetector::detectSDCard() {
    ESP_LOGI(TAG, "Detecting SD card...");
    
    // Configure SD card CS pin as input with pullup
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << SD_CS_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    
    // SD card detect: if CS pin is low when pulled high, card might be present
    // This is a simple heuristic - proper detection requires SPI initialization
    vTaskDelay(pdMS_TO_TICKS(10));
    int level = gpio_get_level((gpio_num_t)SD_CS_PIN);
    
    // For now, we'll consider SD card detection inconclusive via GPIO alone
    // Full detection requires attempting SPI communication
    config.sdCardPresent = false;  // Will be updated when we actually try to mount
    
    ESP_LOGI(TAG, "SD card CS pin level: %d (full detection requires SPI init)", level);
    return false;
}

bool HardwareDetector::detectAccelerometer() {
    ESP_LOGI(TAG, "Detecting accelerometer...");
    
    // Check common accelerometer I2C addresses
    if (checkI2CDevice(MPU6050_I2C_ADDR)) {
        config.accelerometerPresent = true;
        config.accelAddress = MPU6050_I2C_ADDR;
        ESP_LOGI(TAG, "MPU6050 accelerometer detected at 0x%02X", MPU6050_I2C_ADDR);
        return true;
    }
    
    if (checkI2CDevice(ADXL345_I2C_ADDR)) {
        config.accelerometerPresent = true;
        config.accelAddress = ADXL345_I2C_ADDR;
        ESP_LOGI(TAG, "ADXL345 accelerometer detected at 0x%02X", ADXL345_I2C_ADDR);
        return true;
    }
    
    config.accelerometerPresent = false;
    config.accelAddress = 0;
    ESP_LOGI(TAG, "No accelerometer detected");
    return false;
}

HardwareConfig HardwareDetector::scanAll() {
    ESP_LOGI(TAG, "=== Starting Hardware Detection ===");
    
    // Reset config
    memset(&config, 0, sizeof(config));
    
    // Initialize I2C for scanning
    initI2C();
    
    // Scan I2C bus for all devices
    uint8_t foundDevices[10];
    scanI2CBus(foundDevices, 10);
    
    // Detect specific hardware
    detectDisplay();
    detectSDCard();
    detectAccelerometer();
    
    ESP_LOGI(TAG, "=== Hardware Detection Complete ===");
    ESP_LOGI(TAG, "  Display:       %s", config.displayPresent ? "YES" : "NO");
    ESP_LOGI(TAG, "  SD Card:       %s", config.sdCardPresent ? "YES" : "NO");
    ESP_LOGI(TAG, "  Accelerometer: %s", config.accelerometerPresent ? "YES" : "NO");
    
    // Keep I2C initialized for future use
    // deinitI2C();
    
    return config;
}
