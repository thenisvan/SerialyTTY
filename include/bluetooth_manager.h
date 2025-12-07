#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include "config.h"
#include <string>
#include "esp_log.h"

// Forward declarations for BLE types (to be implemented later when BLE is properly configured)
typedef int esp_gatts_cb_event_t;
typedef int esp_gatt_if_t;
typedef struct esp_ble_gatts_cb_param {} esp_ble_gatts_cb_param_t;
typedef int esp_gap_ble_cb_event_t;
typedef struct esp_ble_gap_cb_param {} esp_ble_gap_cb_param_t;
typedef struct esp_ble_adv_params {} esp_ble_adv_params_t;

// BLE connection status (ESP32-C6 supports BLE only, not Classic)
enum BluetoothStatus {
    BLE_DISABLED,
    BLE_INITIALIZING,
    BLE_READY,
    BLE_ADVERTISING,
    BLE_CONNECTED,
    BLE_ERROR
};

class BluetoothManager {
public:
    BluetoothManager();
    
    // Core functionality (BLE UART Service for ESP32-C6)
    bool begin(const char* device_name = "SmvIT-Bridge");
    void end();
    void handleLoop();  // Process BLE events and data
    
    // Connection management
    bool startAdvertising();
    bool stopAdvertising();
    bool isConnected();
    
    // Data transmission (Nordic UART Service compatible)
    size_t write(const uint8_t* data, size_t size);
    size_t write(const char* str);
    int read();
    size_t readBytes(uint8_t* buffer, size_t length);
    int available();
    void flush();
    
    // Status and info
    BluetoothStatus getStatus();
    std::string getDeviceName();
    std::string getConnectedDeviceName();
    std::string getMacAddress();
    
    // Callbacks (set by main application)
    void setOnConnected(void (*callback)());
    void setOnDisconnected(void (*callback)());
    void setOnDataReceived(void (*callback)(const uint8_t* data, size_t length));

private:
    BluetoothStatus status;
    std::string deviceName;
    std::string connectedDevice;
    bool initialized;
    
    // Callback function pointers
    void (*onConnectedCallback)();
    void (*onDisconnectedCallback)();
    void (*onDataReceivedCallback)(const uint8_t* data, size_t length);
    
    // Internal ESP-IDF BLE management (ESP32-C6 compatible)
    void createService();
    void addCharacteristics();
    static void gattsEventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    static void gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
    
    // BLE service and characteristic handles
    esp_gatt_if_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    uint16_t char_tx_handle;
    uint16_t char_rx_handle;
    
    // Advertising parameters
    esp_ble_adv_params_t adv_params;
};

#endif // BLUETOOTH_MANAGER_H