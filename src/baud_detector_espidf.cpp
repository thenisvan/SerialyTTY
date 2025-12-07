#include "baud_detector.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <algorithm>

static const char *TAG = "BAUD_DETECT";

// Static pointer for ISR
static BaudDetector* baudDetectorInstance = nullptr;

BaudDetector::BaudDetector() {
    detectedBaud = 0;
    edgeCount = 0;
    capturingTiming = false;
    baudDetectorInstance = this;
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
        .rx_flow_ctrl_thresh = 0,
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

// GPIO ISR handler for edge detection
void IRAM_ATTR BaudDetector::gpioIsrHandler(void* arg) {
    BaudDetector* detector = (BaudDetector*)arg;
    
    if (detector->capturingTiming && detector->edgeCount < 32) {
        detector->edgeTimestamps[detector->edgeCount++] = esp_timer_get_time();
    }
}

bool BaudDetector::startBitTimingCapture() {
    ESP_LOGI(TAG, "Starting bit timing capture on RX pin...");
    
    // Reset capture state
    edgeCount = 0;
    capturingTiming = true;
    
    // Configure RX pin as input with interrupt on any edge
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << RX_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    
    // Install GPIO ISR service and add handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add((gpio_num_t)RX_PIN, gpioIsrHandler, (void*)this);
    
    return true;
}

void BaudDetector::stopBitTimingCapture() {
    capturingTiming = false;
    gpio_isr_handler_remove((gpio_num_t)RX_PIN);
    gpio_uninstall_isr_service();
    ESP_LOGI(TAG, "Stopped bit timing capture, captured %d edges", edgeCount);
}

uint32_t BaudDetector::calculateBaudFromEdges() {
    if (edgeCount < 4) {
        ESP_LOGW(TAG, "Not enough edges captured for baud calculation");
        return 0;
    }
    
    // Calculate bit periods from edge transitions
    uint64_t bitPeriods[30];
    uint8_t periodCount = 0;
    
    for (uint8_t i = 1; i < edgeCount && periodCount < 30; i++) {
        uint64_t period = edgeTimestamps[i] - edgeTimestamps[i-1];
        // Filter out noise (periods < 10µs or > 10ms)
        if (period > 10 && period < 10000) {
            bitPeriods[periodCount++] = period;
        }
    }
    
    if (periodCount < 3) {
        ESP_LOGW(TAG, "Not enough valid bit periods");
        return 0;
    }
    
    // Find minimum bit period (fastest transition = single bit)
    uint64_t minPeriod = bitPeriods[0];
    for (uint8_t i = 1; i < periodCount; i++) {
        if (bitPeriods[i] < minPeriod) {
            minPeriod = bitPeriods[i];
        }
    }
    
    // Calculate baud rate: baud = 1 / bit_period
    // minPeriod is in microseconds, so baud = 1,000,000 / minPeriod
    uint32_t calculatedBaud = 1000000 / minPeriod;
    
    ESP_LOGI(TAG, "Min bit period: %llu µs, calculated baud: %lu", minPeriod, (unsigned long)calculatedBaud);
    
    // Round to nearest standard baud rate
    uint32_t closestBaud = 0;
    uint32_t minDiff = UINT32_MAX;
    
    for (size_t i = 0; i < NUM_BAUDRATES; i++) {
        uint32_t diff = (calculatedBaud > BAUDRATES[i]) ? 
                       (calculatedBaud - BAUDRATES[i]) : 
                       (BAUDRATES[i] - calculatedBaud);
        
        if (diff < minDiff) {
            minDiff = diff;
            closestBaud = BAUDRATES[i];
        }
    }
    
    // Only accept if within 10% tolerance
    if (minDiff < (closestBaud / 10)) {
        ESP_LOGI(TAG, "Matched to standard baud rate: %lu", (unsigned long)closestBaud);
        return closestBaud;
    }
    
    ESP_LOGW(TAG, "Calculated baud %lu doesn't match standard rates closely enough", 
             (unsigned long)calculatedBaud);
    return 0;
}

uint32_t BaudDetector::detectBaudRateByTiming() {
    ESP_LOGI(TAG, "Starting intelligent baud detection by timing analysis...");
    
    // Start capturing edge timing
    if (!startBitTimingCapture()) {
        ESP_LOGE(TAG, "Failed to start bit timing capture");
        return 0;
    }
    
    // Wait for data (up to 3 seconds)
    ESP_LOGI(TAG, "Waiting for serial data...");
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    // Stop capture
    stopBitTimingCapture();
    
    // Calculate baud rate from captured edges
    uint32_t baud = calculateBaudFromEdges();
    
    if (baud > 0) {
        detectedBaud = baud;
        ESP_LOGI(TAG, "Intelligent detection successful: %lu baud", (unsigned long)baud);
    } else {
        ESP_LOGW(TAG, "Intelligent detection failed, falling back to common rates");
        detectedBaud = tryCommonBaudRates();
    }
    
    return detectedBaud;
}
