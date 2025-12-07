#include "bridge_mode.h"
#include "config.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include <cstring>

static const char *TAG = "BRIDGE";

BridgeMode::BridgeMode() :
    active(false),
    currentBaud(0),
    bytesReceived(0),
    bytesSent(0),
    escapeIndex(0),
    lastEscapeChar(0)
{
    memset(escapeBuffer, 0, sizeof(escapeBuffer));
}

void BridgeMode::enter(uint32_t baudRate) {
    if (active) {
        ESP_LOGW(TAG, "Bridge mode already active");
        return;
    }
    
    currentBaud = baudRate;
    active = true;
    resetStats();
    resetEscapeDetection();
    
    ESP_LOGI(TAG, "=== BRIDGE MODE ACTIVATED ===");
    ESP_LOGI(TAG, "Baud rate: %lu", (unsigned long)baudRate);
    ESP_LOGI(TAG, "Escape sequence: %s", ESCAPE_SEQUENCE);
    ESP_LOGI(TAG, "Type '%s' to return to menu", ESCAPE_SEQUENCE);
    
    // Print to USB serial
    const char* msg = "\r\n=== BRIDGE MODE ===\r\n"
                      "Transparent passthrough active\r\n"
                      "Type '~~~' to exit\r\n\r\n";
    uart_write_bytes(UART_NUM_0, msg, strlen(msg));
}

void BridgeMode::exit() {
    if (!active) {
        return;
    }
    
    active = false;
    
    ESP_LOGI(TAG, "=== BRIDGE MODE DEACTIVATED ===");
    ESP_LOGI(TAG, "RX bytes: %lu, TX bytes: %lu", 
             (unsigned long)bytesReceived, (unsigned long)bytesSent);
    
    // Print exit message to USB
    const char* msg = "\r\n\r\n=== EXITED BRIDGE MODE ===\r\n"
                      "Returning to menu...\r\n\r\n";
    uart_write_bytes(UART_NUM_0, msg, strlen(msg));
}

void BridgeMode::resetStats() {
    bytesReceived = 0;
    bytesSent = 0;
}

void BridgeMode::resetEscapeDetection() {
    escapeIndex = 0;
    lastEscapeChar = 0;
    memset(escapeBuffer, 0, sizeof(escapeBuffer));
}

bool BridgeMode::checkEscapeSequence(char c) {
    uint64_t now = esp_timer_get_time();
    
    // Check if too much time passed since last character
    if (escapeIndex > 0 && (now - lastEscapeChar) > (ESCAPE_TIMEOUT_MS * 1000)) {
        resetEscapeDetection();
    }
    
    lastEscapeChar = now;
    
    // Check if character matches expected position in sequence
    if (c == ESCAPE_SEQUENCE[escapeIndex]) {
        escapeBuffer[escapeIndex++] = c;
        
        // Full sequence detected?
        if (escapeIndex == ESCAPE_SEQ_LEN) {
            ESP_LOGI(TAG, "Escape sequence detected!");
            resetEscapeDetection();
            return true;
        }
    } else {
        // Character doesn't match - reset and check if it's first char
        if (c == ESCAPE_SEQUENCE[0]) {
            escapeBuffer[0] = c;
            escapeIndex = 1;
        } else {
            resetEscapeDetection();
        }
    }
    
    return false;
}

void BridgeMode::forwardUsbToTarget(const uint8_t* data, size_t len) {
    // Forward data from USB to target device (UART_NUM_1)
    int written = uart_write_bytes(UART_NUM_1, data, len);
    if (written > 0) {
        bytesSent += written;
    }
}

void BridgeMode::forwardTargetToUsb(const uint8_t* data, size_t len) {
    // Forward data from target device to USB (UART_NUM_0)
    int written = uart_write_bytes(UART_NUM_0, data, len);
    if (written > 0) {
        bytesReceived += written;
    }
}

void BridgeMode::handleData() {
    if (!active) {
        return;
    }
    
    // Read from USB (UART_NUM_0) and forward to target
    uint8_t usbData[128];
    int usbLen = uart_read_bytes(UART_NUM_0, usbData, sizeof(usbData), pdMS_TO_TICKS(10));
    
    if (usbLen > 0) {
        // Check for escape sequence in USB data
        bool escapeDetected = false;
        size_t forwardStart = 0;
        
        for (int i = 0; i < usbLen; i++) {
            if (checkEscapeSequence(usbData[i])) {
                // Escape sequence detected - forward data up to this point
                if (i > forwardStart) {
                    forwardUsbToTarget(&usbData[forwardStart], i - forwardStart - ESCAPE_SEQ_LEN + 1);
                }
                escapeDetected = true;
                break;
            }
        }
        
        if (escapeDetected) {
            exit();
            return;
        }
        
        // No escape sequence - forward all data
        forwardUsbToTarget(usbData, usbLen);
    }
    
    // Read from target (UART_NUM_1) and forward to USB
    uint8_t targetData[128];
    int targetLen = uart_read_bytes(UART_NUM_1, targetData, sizeof(targetData), pdMS_TO_TICKS(10));
    
    if (targetLen > 0) {
        forwardTargetToUsb(targetData, targetLen);
    }
}
