#include "bluetooth_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BLE_UART";

// Static instance pointer for callbacks
static BluetoothManager* instance = nullptr;

// Profile attributes
#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x55

#define GATTS_SERVICE_UUID_TEST     0x00FF
#define GATTS_CHAR_UUID_TEST_A      0xFF01
#define GATTS_NUM_HANDLE_TEST       8

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500

// Define the service UUID (using Nordic UART Service compatible UUIDs)
constexpr uint8_t BluetoothManager::SERVICE_UUID[16];
constexpr uint8_t BluetoothManager::CHAR_RX_UUID[16];
constexpr uint8_t BluetoothManager::CHAR_TX_UUID[16];

BluetoothManager::BluetoothManager() :
    status(BLE_DISABLED),
    deviceName("SmvIT-Bridge"),
    initialized(false),
    onConnectedCallback(nullptr),
    onDisconnectedCallback(nullptr),
    onDataReceivedCallback(nullptr),
    gatts_if(ESP_GATT_IF_NONE),
    app_id(ESP_APP_ID),
    conn_id(0),
    service_handle(0),
    char_tx_handle(0),
    char_rx_handle(0),
    descr_handle(0),
    rxBufferLen(0)
{
    instance = this;
    memset(rxBuffer, 0, RX_BUFFER_SIZE);
    
    // Initialize advertising parameters
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.adv_int_min = 0x20;
    adv_params.adv_int_max = 0x40;
    adv_params.adv_type = ADV_TYPE_IND;
    adv_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
    adv_params.channel_map = ADV_CHNL_ALL;
    adv_params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
}

bool BluetoothManager::begin(const char* device_name) {
    if (initialized) {
        ESP_LOGW(TAG, "BLE already initialized");
        return true;
    }

    deviceName = device_name;
    status = BLE_INITIALIZING;
    
    ESP_LOGI(TAG, "Initializing BLE UART Service...");

    // Release Bluetooth classic memory (ESP32-C6 is BLE only)
    esp_err_t ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "BT controller mem release failed: %s", esp_err_to_name(ret));
    }

    // Initialize Bluetooth controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BT controller init failed: %s", esp_err_to_name(ret));
        status = BLE_ERROR;
        return false;
    }

    // Enable BLE mode
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BT controller enable failed: %s", esp_err_to_name(ret));
        status = BLE_ERROR;
        return false;
    }

    // Initialize Bluedroid stack
    ret = esp_bluedroid_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluedroid init failed: %s", esp_err_to_name(ret));
        status = BLE_ERROR;
        return false;
    }

    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluedroid enable failed: %s", esp_err_to_name(ret));
        status = BLE_ERROR;
        return false;
    }

    // Register GATT callback
    ret = esp_ble_gatts_register_callback(gattsEventHandler);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GATTS register callback failed: %s", esp_err_to_name(ret));
        status = BLE_ERROR;
        return false;
    }

    // Register GAP callback
    ret = esp_ble_gap_register_callback(gapEventHandler);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GAP register callback failed: %s", esp_err_to_name(ret));
        status = BLE_ERROR;
        return false;
    }

    // Register GATT application
    ret = esp_ble_gatts_app_register(app_id);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GATTS app register failed: %s", esp_err_to_name(ret));
        status = BLE_ERROR;
        return false;
    }

    // Set MTU
    ret = esp_ble_gatt_set_local_mtu(500);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Set local MTU failed: %s", esp_err_to_name(ret));
    }

    initialized = true;
    status = BLE_READY;
    ESP_LOGI(TAG, "BLE initialized successfully as '%s'", device_name);
    return true;
}

void BluetoothManager::end() {
    if (!initialized) return;
    
    ESP_LOGI(TAG, "Deinitializing BLE...");
    
    // Stop advertising if active
    if (status == BLE_ADVERTISING) {
        esp_ble_gap_stop_advertising();
    }
    
    // Unregister app
    if (gatts_if != ESP_GATT_IF_NONE) {
        esp_ble_gatts_app_unregister(gatts_if);
    }
    
    // Disable and deinit Bluedroid
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    
    // Disable and deinit controller
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    
    initialized = false;
    status = BLE_DISABLED;
    gatts_if = ESP_GATT_IF_NONE;
    conn_id = 0;
    
    ESP_LOGI(TAG, "BLE deinitialized");
}

void BluetoothManager::handleLoop() {
    // BLE events are handled via callbacks
    // This method is kept for compatibility with main loop
}

bool BluetoothManager::startAdvertising() {
    if (!initialized || status == BLE_ERROR) {
        ESP_LOGW(TAG, "BLE not ready for advertising");
        return false;
    }

    if (status == BLE_ADVERTISING) {
        ESP_LOGD(TAG, "Already advertising");
        return true;
    }

    esp_err_t ret = esp_ble_gap_start_advertising(&adv_params);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Start advertising failed: %s", esp_err_to_name(ret));
        return false;
    }

    ESP_LOGI(TAG, "Advertising started");
    return true;
}

bool BluetoothManager::stopAdvertising() {
    if (status != BLE_ADVERTISING) {
        return true;
    }

    esp_err_t ret = esp_ble_gap_stop_advertising();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Stop advertising failed: %s", esp_err_to_name(ret));
        return false;
    }

    status = BLE_READY;
    ESP_LOGI(TAG, "Advertising stopped");
    return true;
}

bool BluetoothManager::isConnected() {
    return status == BLE_CONNECTED;
}

size_t BluetoothManager::write(const uint8_t* data, size_t size) {
    if (!isConnected() || char_tx_handle == 0) {
        ESP_LOGD(TAG, "BLE not connected or TX char not ready");
        return 0;
    }

    // Send notification to client
    esp_err_t ret = esp_ble_gatts_send_indicate(
        gatts_if, conn_id, char_tx_handle,
        size, (uint8_t*)data, false
    );

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Send notify failed: %s", esp_err_to_name(ret));
        return 0;
    }

    ESP_LOGD(TAG, "Sent %d bytes via BLE", size);
    return size;
}

size_t BluetoothManager::write(const char* str) {
    return write((const uint8_t*)str, strlen(str));
}

int BluetoothManager::read() {
    if (rxBufferLen == 0) {
        return -1;
    }
    
    uint8_t byte = rxBuffer[0];
    // Shift buffer
    memmove(rxBuffer, rxBuffer + 1, rxBufferLen - 1);
    rxBufferLen--;
    
    return byte;
}

size_t BluetoothManager::readBytes(uint8_t* buffer, size_t length) {
    if (rxBufferLen == 0) {
        return 0;
    }
    
    size_t to_read = (length < rxBufferLen) ? length : rxBufferLen;
    memcpy(buffer, rxBuffer, to_read);
    
    // Shift buffer
    memmove(rxBuffer, rxBuffer + to_read, rxBufferLen - to_read);
    rxBufferLen -= to_read;
    
    return to_read;
}

int BluetoothManager::available() {
    return rxBufferLen;
}

void BluetoothManager::flush() {
    // No buffering in BLE implementation
}

BluetoothStatus BluetoothManager::getStatus() {
    return status;
}

std::string BluetoothManager::getDeviceName() {
    return deviceName;
}

void BluetoothManager::setOnConnected(void (*callback)()) {
    onConnectedCallback = callback;
}

void BluetoothManager::setOnDisconnected(void (*callback)()) {
    onDisconnectedCallback = callback;
}

void BluetoothManager::setOnDataReceived(void (*callback)(const uint8_t* data, size_t length)) {
    onDataReceivedCallback = callback;
}

void BluetoothManager::gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    if (!instance) return;

    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "Advertising data set, starting advertising");
            esp_ble_gap_start_advertising(&instance->adv_params);
            break;

        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
                ESP_LOGI(TAG, "Advertising started successfully");
                instance->status = BLE_ADVERTISING;
            } else {
                ESP_LOGE(TAG, "Advertising start failed");
            }
            break;

        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status == ESP_BT_STATUS_SUCCESS) {
                ESP_LOGI(TAG, "Advertising stopped successfully");
                instance->status = BLE_READY;
            }
            break;

        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(TAG, "Connection params updated: status=%d, min_int=%d, max_int=%d, latency=%d, timeout=%d",
                param->update_conn_params.status,
                param->update_conn_params.min_int,
                param->update_conn_params.max_int,
                param->update_conn_params.latency,
                param->update_conn_params.timeout);
            break;

        default:
            ESP_LOGD(TAG, "GAP event: %d", event);
            break;
    }
}

void BluetoothManager::gattsEventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    if (!instance) return;

    // Store interface if this is our app
    if (event == ESP_GATTS_REG_EVT && param->reg.app_id == instance->app_id) {
        instance->gatts_if = gatts_if;
    }

    // Ignore events for other apps
    if (gatts_if != ESP_GATT_IF_NONE && gatts_if != instance->gatts_if) {
        return;
    }

    switch (event) {
        case ESP_GATTS_REG_EVT:
            ESP_LOGI(TAG, "GATT server registered, app_id=%d", param->reg.app_id);
            
            // Set device name
            esp_ble_gap_set_device_name(instance->deviceName.c_str());
            
            // Configure advertising data
            esp_ble_adv_data_t adv_data = {};
            adv_data.set_scan_rsp = false;
            adv_data.include_name = true;
            adv_data.include_txpower = true;
            adv_data.min_interval = 0x0006;
            adv_data.max_interval = 0x0010;
            adv_data.appearance = 0x00;
            adv_data.manufacturer_len = 0;
            adv_data.p_manufacturer_data = nullptr;
            adv_data.service_data_len = 0;
            adv_data.p_service_data = nullptr;
            adv_data.service_uuid_len = 16;
            adv_data.p_service_uuid = (uint8_t*)SERVICE_UUID;
            adv_data.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
            
            esp_ble_gap_config_adv_data(&adv_data);
            
            // Create GATT service
            instance->createService();
            break;

        case ESP_GATTS_CREATE_EVT:
            ESP_LOGI(TAG, "Service created, handle=%d", param->create.service_handle);
            instance->service_handle = param->create.service_handle;
            esp_ble_gatts_start_service(instance->service_handle);
            
            // Add TX characteristic (notify)
            esp_bt_uuid_t tx_uuid;
            tx_uuid.len = ESP_UUID_LEN_128;
            memcpy(tx_uuid.uuid.uuid128, CHAR_TX_UUID, 16);
            
            esp_ble_gatts_add_char(instance->service_handle, &tx_uuid,
                ESP_GATT_PERM_READ,
                ESP_GATT_CHAR_PROP_BIT_NOTIFY,
                nullptr, nullptr);
            break;

        case ESP_GATTS_ADD_CHAR_EVT: {
            ESP_LOGI(TAG, "Characteristic added, handle=%d", param->add_char.attr_handle);
            
            if (instance->char_tx_handle == 0) {
                // This is TX characteristic
                instance->char_tx_handle = param->add_char.attr_handle;
                
                // Add RX characteristic (write)
                esp_bt_uuid_t rx_uuid;
                rx_uuid.len = ESP_UUID_LEN_128;
                memcpy(rx_uuid.uuid.uuid128, CHAR_RX_UUID, 16);
                
                esp_ble_gatts_add_char(instance->service_handle, &rx_uuid,
                    ESP_GATT_PERM_WRITE,
                    ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_WRITE_NR,
                    nullptr, nullptr);
            } else {
                // This is RX characteristic
                instance->char_rx_handle = param->add_char.attr_handle;
            }
            break;
        }

        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(TAG, "Client connected, conn_id=%d", param->connect.conn_id);
            instance->conn_id = param->connect.conn_id;
            instance->status = BLE_CONNECTED;
            
            if (instance->onConnectedCallback) {
                instance->onConnectedCallback();
            }
            
            // Update connection parameters
            esp_ble_conn_update_params_t conn_params = {};
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            conn_params.latency = 0;
            conn_params.max_int = 0x10;
            conn_params.min_int = 0x08;
            conn_params.timeout = 400;
            esp_ble_gap_update_conn_params(&conn_params);
            break;

        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(TAG, "Client disconnected, reason=%d", param->disconnect.reason);
            instance->status = BLE_READY;
            instance->conn_id = 0;
            
            if (instance->onDisconnectedCallback) {
                instance->onDisconnectedCallback();
            }
            
            // Restart advertising
            instance->startAdvertising();
            break;

        case ESP_GATTS_WRITE_EVT:
            if (param->write.handle == instance->char_rx_handle) {
                ESP_LOGD(TAG, "Received %d bytes on RX characteristic", param->write.len);
                
                // Store in buffer
                size_t space = RX_BUFFER_SIZE - instance->rxBufferLen;
                size_t to_copy = (param->write.len < space) ? param->write.len : space;
                
                if (to_copy > 0) {
                    memcpy(instance->rxBuffer + instance->rxBufferLen, param->write.value, to_copy);
                    instance->rxBufferLen += to_copy;
                    
                    // Trigger callback
                    if (instance->onDataReceivedCallback) {
                        instance->onDataReceivedCallback(param->write.value, param->write.len);
                    }
                }
                
                if (param->write.need_rsp) {
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, 
                        param->write.trans_id, ESP_GATT_OK, nullptr);
                }
            }
            break;

        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(TAG, "MTU changed to %d", param->mtu.mtu);
            break;

        default:
            ESP_LOGD(TAG, "GATTS event: %d", event);
            break;
    }
}

void BluetoothManager::createService() {
    esp_gatt_srvc_id_t service_id;
    service_id.is_primary = true;
    service_id.id.inst_id = 0x00;
    service_id.id.uuid.len = ESP_UUID_LEN_128;
    memcpy(service_id.id.uuid.uuid.uuid128, SERVICE_UUID, 16);

    esp_ble_gatts_create_service(gatts_if, &service_id, GATTS_NUM_HANDLE_TEST);
}

void BluetoothManager::addCharacteristics() {
    // Stub implementation - will be implemented when BLE is properly configured
    ESP_LOGD(TAG, "BLE add characteristics stub called");
}