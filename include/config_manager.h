#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <cstdint>
#include <cstring>
#include "esp_err.h"

// Maximum string lengths for configuration
#define MAX_DEVICE_NAME_LEN 32
#define MAX_BLE_NAME_LEN 32
#define MAX_BLE_PIN_LEN 7
#define MAX_ESCAPE_SEQ_LEN 8

// Configuration structures for each subsystem

struct UARTSettings {
    uint32_t baudRate;          // Baud rate (9600-115200)
    bool autoDetect;            // Enable auto-detection
    uint8_t parity;             // 0=None, 1=Even, 2=Odd
    uint8_t stopBits;           // 1 or 2
    uint8_t flowControl;        // 0=None, 1=RTS/CTS, 2=XON/XOFF
    
    // Defaults
    UARTSettings() : 
        baudRate(115200),
        autoDetect(true),
        parity(0),
        stopBits(1),
        flowControl(0) {}
};

struct BluetoothSettings {
    bool enabled;               // BLE enabled/disabled
    char deviceName[MAX_BLE_NAME_LEN];  // BLE device name
    bool autoAdvertise;         // Start advertising on boot
    bool requirePairing;        // Require pairing for connection
    char pinCode[MAX_BLE_PIN_LEN];  // 6-digit PIN
    uint16_t advertisingInterval;   // Advertising interval (ms)
    int8_t txPower;             // TX power in dBm
    
    // Defaults
    BluetoothSettings() : 
        enabled(false),
        autoAdvertise(false),
        requirePairing(false),
        advertisingInterval(100),
        txPower(0) {
        strcpy(deviceName, "SmvIT-Bridge");
        strcpy(pinCode, "123456");
    }
};

struct DisplaySettings {
    bool enabled;               // Display enabled/disabled
    uint8_t brightness;         // 0-255
    uint16_t timeout;           // Screen timeout in seconds (0=never)
    uint8_t rotation;           // 0-3 (0°, 90°, 180°, 270°)
    bool autoOff;               // Auto-off when inactive
    
    // Defaults
    DisplaySettings() :
        enabled(true),
        brightness(200),
        timeout(300),
        rotation(0),
        autoOff(true) {}
};

struct LoggingSettings {
    bool enabled;               // Logging enabled/disabled
    uint8_t logLevel;           // 0=None, 1=Error, 2=Warn, 3=Info, 4=Debug, 5=Verbose
    uint32_t maxFileSizeKB;     // Max log file size in KB
    bool autoRotate;            // Auto-rotate logs when full
    bool logToSerial;           // Also log to serial console
    
    // Defaults
    LoggingSettings() :
        enabled(true),
        logLevel(3),
        maxFileSizeKB(1024),
        autoRotate(true),
        logToSerial(true) {}
};

struct SystemSettings {
    char deviceName[MAX_DEVICE_NAME_LEN];  // Device identifier
    int16_t timezoneOffset;     // Timezone offset in minutes
    bool ntpEnabled;            // NTP time sync enabled
    char escapeSequence[MAX_ESCAPE_SEQ_LEN];  // Bridge mode escape
    bool debugMode;             // Enable debug features
    
    // Defaults
    SystemSettings() :
        timezoneOffset(0),
        ntpEnabled(false),
        debugMode(false) {
        strcpy(deviceName, "SmvIT-Bridge");
        strcpy(escapeSequence, "~~~");
    }
};

// Main configuration structure
struct DeviceConfig {
    UARTSettings uart;
    BluetoothSettings bluetooth;
    DisplaySettings display;
    LoggingSettings logging;
    SystemSettings system;
    
    // Configuration version for migration
    uint16_t configVersion;
    
    DeviceConfig() : configVersion(1) {}
};

// Configuration Manager class
class ConfigManager {
public:
    // Singleton pattern
    static ConfigManager& getInstance();
    
    // Lifecycle
    bool begin();               // Initialize NVS and load settings
    bool save();                // Save all settings to NVS
    bool load();                // Load all settings from NVS
    bool reset();               // Reset to factory defaults
    
    // Get configuration reference (read-only)
    const DeviceConfig& getConfig() const { return config; }
    
    // UART Settings
    bool setUARTBaudRate(uint32_t baudRate);
    bool setUARTAutoDetect(bool enabled);
    bool setUARTParity(uint8_t parity);
    bool setUARTStopBits(uint8_t stopBits);
    bool setUARTFlowControl(uint8_t flowControl);
    
    // Bluetooth Settings
    bool setBluetoothEnabled(bool enabled);
    bool setBluetoothDeviceName(const char* name);
    bool setBluetoothAutoAdvertise(bool enabled);
    bool setBluetoothRequirePairing(bool required);
    bool setBluetoothPinCode(const char* pin);
    bool setBluetoothAdvertisingInterval(uint16_t intervalMs);
    bool setBluetoothTxPower(int8_t power);
    
    // Display Settings
    bool setDisplayEnabled(bool enabled);
    bool setDisplayBrightness(uint8_t brightness);
    bool setDisplayTimeout(uint16_t seconds);
    bool setDisplayRotation(uint8_t rotation);
    bool setDisplayAutoOff(bool enabled);
    
    // Logging Settings
    bool setLoggingEnabled(bool enabled);
    bool setLoggingLevel(uint8_t level);
    bool setLoggingMaxFileSize(uint32_t sizeKB);
    bool setLoggingAutoRotate(bool enabled);
    bool setLoggingToSerial(bool enabled);
    
    // System Settings
    bool setSystemDeviceName(const char* name);
    bool setSystemTimezoneOffset(int16_t offset);
    bool setSystemNTPEnabled(bool enabled);
    bool setSystemEscapeSequence(const char* sequence);
    bool setSystemDebugMode(bool enabled);
    
    // Change notification callback
    typedef void (*OnSettingsChangedCallback)();
    void setOnSettingsChanged(OnSettingsChangedCallback callback);
    
    // Utilities
    bool isInitialized() const { return initialized; }
    bool exportToJSON(char* buffer, size_t bufferSize);
    bool importFromJSON(const char* jsonString);
    
private:
    ConfigManager();
    ~ConfigManager();
    
    // Prevent copying
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    // Internal methods
    bool loadFromNVS();
    bool saveToNVS();
    bool validateConfig();
    void notifySettingsChanged();
    
    // NVS helpers
    bool readString(const char* key, char* outValue, size_t maxLen);
    bool writeString(const char* key, const char* value);
    bool readUInt8(const char* key, uint8_t* outValue);
    bool writeUInt8(const char* key, uint8_t value);
    bool readUInt16(const char* key, uint16_t* outValue);
    bool writeUInt16(const char* key, uint16_t value);
    bool readUInt32(const char* key, uint32_t* outValue);
    bool writeUInt32(const char* key, uint32_t value);
    bool readInt16(const char* key, int16_t* outValue);
    bool writeInt16(const char* key, int16_t value);
    bool readInt8(const char* key, int8_t* outValue);
    bool writeInt8(const char* key, int8_t value);
    
    // State
    DeviceConfig config;
    bool initialized;
    OnSettingsChangedCallback onChangedCallback;
    
    // NVS handle
    uint32_t nvsHandle;
};

#endif // CONFIG_MANAGER_H
