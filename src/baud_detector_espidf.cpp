#include "baud_detector.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BAUD_DETECT";

BaudDetector::BaudDetector() {
    detectedBaud = 0;
}

void BaudDetector::begin() {
    ESP_LOGI(TAG, "Baud detector initialized");
    // UART will be configured dynamically when testing baud rates
}

uint32_t BaudDetector::detectBaudRate(uint32_t timeout) {
    ESP_LOGI(TAG, "Starting baud rate detection...");
    
    // Try common baud rates
    detectedBaud = tryCommonBaudRates();
    
    if (detectedBaud > 0) {
        ESP_LOGI(TAG, "Detected baud rate: %lu", (unsigned long)detectedBaud);
    } else {
        ESP_LOGW(TAG, "Failed to detect baud rate");
    }
    
    return detectedBaud;
}

bool BaudDetector::testBaudRate(uint32_t baud, uint32_t timeout) {
    // Configure UART with test baud rate
    uart_config_t uart_config = {
        .baud_rate = (int)baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    
    uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0);
    
    // Wait for data
    vTaskDelay(pdMS_TO_TICKS(timeout));
    
    // Check if we received valid data
    size_t available = 0;
    uart_get_buffered_data_len(UART_NUM_1, &available);
    
    uart_driver_delete(UART_NUM_1);
    
    return available > 0;
}

bool BaudDetector::isDataAvailable() {
    // Simple GPIO check on RX pin for activity
    gpio_set_direction((gpio_num_t)RX_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)RX_PIN, GPIO_PULLUP_ONLY);
    
    int state1 = gpio_get_level((gpio_num_t)RX_PIN);
    vTaskDelay(pdMS_TO_TICKS(50));
    int state2 = gpio_get_level((gpio_num_t)RX_PIN);
    
    // If pin changes state, there's likely data
    return state1 != state2;
}

float BaudDetector::analyzeBitPattern() {
    // TODO: Implement bit timing analysis
    return 0.0f;
}

uint32_t BaudDetector::tryCommonBaudRates() {
    for (size_t i = 0; i < NUM_BAUDRATES; i++) {
        ESP_LOGI(TAG, "Testing baud rate: %lu", (unsigned long)BAUDRATES[i]);
        
        if (testBaudRate(BAUDRATES[i], 500)) {
            return BAUDRATES[i];
        }
    }
    
    return 0; // Not detected
}
