#include "comm_tester.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "COMM_TESTER";

CommTester::CommTester() {
    baudRate = 0;
    lastTestTime = 0;
    lastTestResult = false;
    testMessage = TEST_MESSAGE;
}

void CommTester::begin(uint32_t baud) {
    baudRate = baud;
    
    // Configure UART (fields must be in struct declaration order)
    uart_config_t uart_config = {
        .baud_rate = (int)baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, 1024, 0, 0, NULL, 0);
    
    ESP_LOGI(TAG, "Communication tester initialized at %lu baud", (unsigned long)baud);
}

bool CommTester::testCommunication() {
    sendTestMessage();
    lastTestResult = waitForResponse(RESPONSE_TIMEOUT);
    lastTestTime = (uint32_t)(esp_timer_get_time() / 1000ULL);
    return lastTestResult;
}

void CommTester::sendTestMessage() {
    if (baudRate == 0) {
        ESP_LOGW(TAG, "Cannot send test message - not initialized");
        return;
    }
    
    size_t len = testMessage.length();
    int written = uart_write_bytes(UART_NUM_1, testMessage.c_str(), len);
    
    ESP_LOGI(TAG, "Sent test message (%d bytes)", written);
}

bool CommTester::waitForResponse(uint32_t timeout) {
    uint8_t buffer[128];
    
    int len = uart_read_bytes(UART_NUM_1, buffer, sizeof(buffer), pdMS_TO_TICKS(timeout));
    
    if (len > 0) {
        ESP_LOGI(TAG, "Received %d bytes in response", len);
        return true;
    }
    
    ESP_LOGW(TAG, "No response received");
    return false;
}

void CommTester::resetSerial() {
    uart_flush(UART_NUM_1);
}
