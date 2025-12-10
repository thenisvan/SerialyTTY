#include "config_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <cstring>

static const char *TAG = "CONFIG_MGR";
static const char *NVS_NAMESPACE = "smvit_config";

// Singleton instance
ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager() :
    initialized(false),
    onChangedCallback(nullptr),
    nvsHandle(0)
{
}

ConfigManager::~ConfigManager() {
    if (nvsHandle != 0) {
        nvs_close(nvsHandle);
    }
}

bool ConfigManager::begin() {
    ESP_LOGI(TAG, "Initializing Configuration Manager");
    
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(err));
        return false;
    }
    
    // Open NVS handle
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }
    
    // Load configuration from NVS
    if (!load()) {
        ESP_LOGW(TAG, "No saved configuration found, using defaults");
        // Save defaults to NVS
        save();
    }
    
    initialized = true;
    ESP_LOGI(TAG, "Configuration Manager initialized successfully");
    return true;
}

bool ConfigManager::load() {
    ESP_LOGI(TAG, "Loading configuration from NVS");
    
    if (nvsHandle == 0) {
        ESP_LOGE(TAG, "NVS not initialized");
        return false;
    }
    
    // Try to load config version first
    uint16_t version = 0;
    if (readUInt16("cfg_version", &version)) {
        config.configVersion = version;
        ESP_LOGI(TAG, "Config version: %d", version);
    } else {
        ESP_LOGI(TAG, "No saved configuration found");
        return false;
    }
    
    // Load UART settings
    readUInt32("uart_baud", &config.uart.baudRate);
    uint8_t tempU8;
    if (readUInt8("uart_auto", &tempU8)) config.uart.autoDetect = (tempU8 != 0);
    readUInt8("uart_parity", &config.uart.parity);
    readUInt8("uart_stop", &config.uart.stopBits);
    readUInt8("uart_flow", &config.uart.flowControl);
    
    // Load Bluetooth settings
    if (readUInt8("ble_enabled", &tempU8)) config.bluetooth.enabled = (tempU8 != 0);
    readString("ble_name", config.bluetooth.deviceName, MAX_BLE_NAME_LEN);
    if (readUInt8("ble_auto_adv", &tempU8)) config.bluetooth.autoAdvertise = (tempU8 != 0);
    if (readUInt8("ble_pairing", &tempU8)) config.bluetooth.requirePairing = (tempU8 != 0);
    readString("ble_pin", config.bluetooth.pinCode, MAX_BLE_PIN_LEN);
    readUInt16("ble_adv_int", &config.bluetooth.advertisingInterval);
    readInt8("ble_tx_pwr", &config.bluetooth.txPower);
    
    // Load Display settings
    if (readUInt8("disp_enabled", &tempU8)) config.display.enabled = (tempU8 != 0);
    readUInt8("disp_bright", &config.display.brightness);
    readUInt16("disp_timeout", &config.display.timeout);
    readUInt8("disp_rotate", &config.display.rotation);
    if (readUInt8("disp_auto", &tempU8)) config.display.autoOff = (tempU8 != 0);
    
    // Load Logging settings
    if (readUInt8("log_enabled", &tempU8)) config.logging.enabled = (tempU8 != 0);
    readUInt8("log_level", &config.logging.logLevel);
    readUInt32("log_max_size", &config.logging.maxFileSizeKB);
    if (readUInt8("log_rotate", &tempU8)) config.logging.autoRotate = (tempU8 != 0);
    if (readUInt8("log_serial", &tempU8)) config.logging.logToSerial = (tempU8 != 0);
    
    // Load System settings
    readString("sys_name", config.system.deviceName, MAX_DEVICE_NAME_LEN);
    readInt16("sys_tz", &config.system.timezoneOffset);
    if (readUInt8("sys_ntp", &tempU8)) config.system.ntpEnabled = (tempU8 != 0);
    readString("sys_escape", config.system.escapeSequence, MAX_ESCAPE_SEQ_LEN);
    if (readUInt8("sys_debug", &tempU8)) config.system.debugMode = (tempU8 != 0);
    
    // Validate loaded configuration
    if (!validateConfig()) {
        ESP_LOGW(TAG, "Configuration validation failed, using defaults");
        config = DeviceConfig();  // Reset to defaults
        return false;
    }
    
    ESP_LOGI(TAG, "Configuration loaded successfully");
    return true;
}

bool ConfigManager::save() {
    ESP_LOGI(TAG, "Saving configuration to NVS");
    
    if (nvsHandle == 0) {
        ESP_LOGE(TAG, "NVS not initialized");
        return false;
    }
    
    // Validate before saving
    if (!validateConfig()) {
        ESP_LOGE(TAG, "Cannot save invalid configuration");
        return false;
    }
    
    // Save config version
    writeUInt16("cfg_version", config.configVersion);
    
    // Save UART settings
    writeUInt32("uart_baud", config.uart.baudRate);
    writeUInt8("uart_auto", config.uart.autoDetect ? 1 : 0);
    writeUInt8("uart_parity", config.uart.parity);
    writeUInt8("uart_stop", config.uart.stopBits);
    writeUInt8("uart_flow", config.uart.flowControl);
    
    // Save Bluetooth settings
    writeUInt8("ble_enabled", config.bluetooth.enabled ? 1 : 0);
    writeString("ble_name", config.bluetooth.deviceName);
    writeUInt8("ble_auto_adv", config.bluetooth.autoAdvertise ? 1 : 0);
    writeUInt8("ble_pairing", config.bluetooth.requirePairing ? 1 : 0);
    writeString("ble_pin", config.bluetooth.pinCode);
    writeUInt16("ble_adv_int", config.bluetooth.advertisingInterval);
    writeInt8("ble_tx_pwr", config.bluetooth.txPower);
    
    // Save Display settings
    writeUInt8("disp_enabled", config.display.enabled ? 1 : 0);
    writeUInt8("disp_bright", config.display.brightness);
    writeUInt16("disp_timeout", config.display.timeout);
    writeUInt8("disp_rotate", config.display.rotation);
    writeUInt8("disp_auto", config.display.autoOff ? 1 : 0);
    
    // Save Logging settings
    writeUInt8("log_enabled", config.logging.enabled ? 1 : 0);
    writeUInt8("log_level", config.logging.logLevel);
    writeUInt32("log_max_size", config.logging.maxFileSizeKB);
    writeUInt8("log_rotate", config.logging.autoRotate ? 1 : 0);
    writeUInt8("log_serial", config.logging.logToSerial ? 1 : 0);
    
    // Save System settings
    writeString("sys_name", config.system.deviceName);
    writeInt16("sys_tz", config.system.timezoneOffset);
    writeUInt8("sys_ntp", config.system.ntpEnabled ? 1 : 0);
    writeString("sys_escape", config.system.escapeSequence);
    writeUInt8("sys_debug", config.system.debugMode ? 1 : 0);
    
    // Commit changes
    esp_err_t err = nvs_commit(nvsHandle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS: %s", esp_err_to_name(err));
        return false;
    }
    
    ESP_LOGI(TAG, "Configuration saved successfully");
    notifySettingsChanged();
    return true;
}

bool ConfigManager::reset() {
    ESP_LOGI(TAG, "Resetting configuration to factory defaults");
    
    // Reset to defaults
    config = DeviceConfig();
    
    // Save defaults
    return save();
}

bool ConfigManager::validateConfig() {
    // Validate UART settings
    if (config.uart.baudRate < 300 || config.uart.baudRate > 921600) {
        ESP_LOGE(TAG, "Invalid baud rate: %lu", (unsigned long)config.uart.baudRate);
        return false;
    }
    if (config.uart.parity > 2) {
        ESP_LOGE(TAG, "Invalid parity: %d", config.uart.parity);
        return false;
    }
    if (config.uart.stopBits < 1 || config.uart.stopBits > 2) {
        ESP_LOGE(TAG, "Invalid stop bits: %d", config.uart.stopBits);
        return false;
    }
    if (config.uart.flowControl > 2) {
        ESP_LOGE(TAG, "Invalid flow control: %d", config.uart.flowControl);
        return false;
    }
    
    // Validate Bluetooth settings
    if (config.bluetooth.advertisingInterval < 20 || config.bluetooth.advertisingInterval > 10240) {
        ESP_LOGE(TAG, "Invalid advertising interval: %d", config.bluetooth.advertisingInterval);
        return false;
    }
    if (config.bluetooth.txPower < -12 || config.bluetooth.txPower > 9) {
        ESP_LOGE(TAG, "Invalid TX power: %d", config.bluetooth.txPower);
        return false;
    }
    
    // Validate Display settings
    if (config.display.rotation > 3) {
        ESP_LOGE(TAG, "Invalid rotation: %d", config.display.rotation);
        return false;
    }
    
    // Validate Logging settings
    if (config.logging.logLevel > 5) {
        ESP_LOGE(TAG, "Invalid log level: %d", config.logging.logLevel);
        return false;
    }
    
    return true;
}

// UART Settings Setters
bool ConfigManager::setUARTBaudRate(uint32_t baudRate) {
    if (baudRate < 300 || baudRate > 921600) return false;
    config.uart.baudRate = baudRate;
    return save();
}

bool ConfigManager::setUARTAutoDetect(bool enabled) {
    config.uart.autoDetect = enabled;
    return save();
}

bool ConfigManager::setUARTParity(uint8_t parity) {
    if (parity > 2) return false;
    config.uart.parity = parity;
    return save();
}

bool ConfigManager::setUARTStopBits(uint8_t stopBits) {
    if (stopBits < 1 || stopBits > 2) return false;
    config.uart.stopBits = stopBits;
    return save();
}

bool ConfigManager::setUARTFlowControl(uint8_t flowControl) {
    if (flowControl > 2) return false;
    config.uart.flowControl = flowControl;
    return save();
}

// Bluetooth Settings Setters
bool ConfigManager::setBluetoothEnabled(bool enabled) {
    config.bluetooth.enabled = enabled;
    return save();
}

bool ConfigManager::setBluetoothDeviceName(const char* name) {
    if (!name || strlen(name) == 0 || strlen(name) >= MAX_BLE_NAME_LEN) return false;
    strncpy(config.bluetooth.deviceName, name, MAX_BLE_NAME_LEN - 1);
    config.bluetooth.deviceName[MAX_BLE_NAME_LEN - 1] = '\0';
    return save();
}

bool ConfigManager::setBluetoothAutoAdvertise(bool enabled) {
    config.bluetooth.autoAdvertise = enabled;
    return save();
}

bool ConfigManager::setBluetoothRequirePairing(bool required) {
    config.bluetooth.requirePairing = required;
    return save();
}

bool ConfigManager::setBluetoothPinCode(const char* pin) {
    if (!pin || strlen(pin) != 6) return false;
    // Validate PIN is numeric
    for (int i = 0; i < 6; i++) {
        if (pin[i] < '0' || pin[i] > '9') return false;
    }
    strncpy(config.bluetooth.pinCode, pin, MAX_BLE_PIN_LEN - 1);
    config.bluetooth.pinCode[MAX_BLE_PIN_LEN - 1] = '\0';
    return save();
}

bool ConfigManager::setBluetoothAdvertisingInterval(uint16_t intervalMs) {
    if (intervalMs < 20 || intervalMs > 10240) return false;
    config.bluetooth.advertisingInterval = intervalMs;
    return save();
}

bool ConfigManager::setBluetoothTxPower(int8_t power) {
    if (power < -12 || power > 9) return false;
    config.bluetooth.txPower = power;
    return save();
}

// Display Settings Setters
bool ConfigManager::setDisplayEnabled(bool enabled) {
    config.display.enabled = enabled;
    return save();
}

bool ConfigManager::setDisplayBrightness(uint8_t brightness) {
    config.display.brightness = brightness;
    return save();
}

bool ConfigManager::setDisplayTimeout(uint16_t seconds) {
    config.display.timeout = seconds;
    return save();
}

bool ConfigManager::setDisplayRotation(uint8_t rotation) {
    if (rotation > 3) return false;
    config.display.rotation = rotation;
    return save();
}

bool ConfigManager::setDisplayAutoOff(bool enabled) {
    config.display.autoOff = enabled;
    return save();
}

// Logging Settings Setters
bool ConfigManager::setLoggingEnabled(bool enabled) {
    config.logging.enabled = enabled;
    return save();
}

bool ConfigManager::setLoggingLevel(uint8_t level) {
    if (level > 5) return false;
    config.logging.logLevel = level;
    return save();
}

bool ConfigManager::setLoggingMaxFileSize(uint32_t sizeKB) {
    config.logging.maxFileSizeKB = sizeKB;
    return save();
}

bool ConfigManager::setLoggingAutoRotate(bool enabled) {
    config.logging.autoRotate = enabled;
    return save();
}

bool ConfigManager::setLoggingToSerial(bool enabled) {
    config.logging.logToSerial = enabled;
    return save();
}

// System Settings Setters
bool ConfigManager::setSystemDeviceName(const char* name) {
    if (!name || strlen(name) == 0 || strlen(name) >= MAX_DEVICE_NAME_LEN) return false;
    strncpy(config.system.deviceName, name, MAX_DEVICE_NAME_LEN - 1);
    config.system.deviceName[MAX_DEVICE_NAME_LEN - 1] = '\0';
    return save();
}

bool ConfigManager::setSystemTimezoneOffset(int16_t offset) {
    config.system.timezoneOffset = offset;
    return save();
}

bool ConfigManager::setSystemNTPEnabled(bool enabled) {
    config.system.ntpEnabled = enabled;
    return save();
}

bool ConfigManager::setSystemEscapeSequence(const char* sequence) {
    if (!sequence || strlen(sequence) == 0 || strlen(sequence) >= MAX_ESCAPE_SEQ_LEN) return false;
    strncpy(config.system.escapeSequence, sequence, MAX_ESCAPE_SEQ_LEN - 1);
    config.system.escapeSequence[MAX_ESCAPE_SEQ_LEN - 1] = '\0';
    return save();
}

bool ConfigManager::setSystemDebugMode(bool enabled) {
    config.system.debugMode = enabled;
    return save();
}

void ConfigManager::setOnSettingsChanged(OnSettingsChangedCallback callback) {
    onChangedCallback = callback;
}

void ConfigManager::notifySettingsChanged() {
    if (onChangedCallback) {
        onChangedCallback();
    }
}

// NVS Helper Methods
bool ConfigManager::readString(const char* key, char* outValue, size_t maxLen) {
    size_t required_size = maxLen;
    esp_err_t err = nvs_get_str(nvsHandle, key, outValue, &required_size);
    return (err == ESP_OK);
}

bool ConfigManager::writeString(const char* key, const char* value) {
    esp_err_t err = nvs_set_str(nvsHandle, key, value);
    return (err == ESP_OK);
}

bool ConfigManager::readUInt8(const char* key, uint8_t* outValue) {
    esp_err_t err = nvs_get_u8(nvsHandle, key, outValue);
    return (err == ESP_OK);
}

bool ConfigManager::writeUInt8(const char* key, uint8_t value) {
    esp_err_t err = nvs_set_u8(nvsHandle, key, value);
    return (err == ESP_OK);
}

bool ConfigManager::readUInt16(const char* key, uint16_t* outValue) {
    esp_err_t err = nvs_get_u16(nvsHandle, key, outValue);
    return (err == ESP_OK);
}

bool ConfigManager::writeUInt16(const char* key, uint16_t value) {
    esp_err_t err = nvs_set_u16(nvsHandle, key, value);
    return (err == ESP_OK);
}

bool ConfigManager::readUInt32(const char* key, uint32_t* outValue) {
    esp_err_t err = nvs_get_u32(nvsHandle, key, outValue);
    return (err == ESP_OK);
}

bool ConfigManager::writeUInt32(const char* key, uint32_t value) {
    esp_err_t err = nvs_set_u32(nvsHandle, key, value);
    return (err == ESP_OK);
}

bool ConfigManager::readInt16(const char* key, int16_t* outValue) {
    esp_err_t err = nvs_get_i16(nvsHandle, key, outValue);
    return (err == ESP_OK);
}

bool ConfigManager::writeInt16(const char* key, int16_t value) {
    esp_err_t err = nvs_set_i16(nvsHandle, key, value);
    return (err == ESP_OK);
}

bool ConfigManager::readInt8(const char* key, int8_t* outValue) {
    esp_err_t err = nvs_get_i8(nvsHandle, key, outValue);
    return (err == ESP_OK);
}

bool ConfigManager::writeInt8(const char* key, int8_t value) {
    esp_err_t err = nvs_set_i8(nvsHandle, key, value);
    return (err == ESP_OK);
}

bool ConfigManager::exportToJSON(char* buffer, size_t bufferSize) {
    // TODO: Implement JSON export
    ESP_LOGW(TAG, "JSON export not yet implemented");
    return false;
}

bool ConfigManager::importFromJSON(const char* jsonString) {
    // TODO: Implement JSON import
    ESP_LOGW(TAG, "JSON import not yet implemented");
    return false;
}
