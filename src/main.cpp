#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "config.h"
#include "display_manager.h"
#include "baud_detector.h"
#include "sd_logger.h"
#include "comm_tester.h"
#include "bluetooth_manager.h"
#include "hardware_detector.h"
#include "bridge_mode.h"
#include "menu_system.h"

static const char *TAG = "MAIN";

// Global objects
HardwareDetector hwDetector;
DisplayManager display;
SDLogger logger;
BaudDetector baudDetector;
CommTester commTester;
BluetoothManager bluetooth;
BridgeMode bridge;
MenuSystem menu;

// System state
SystemState currentState = STATE_BOOTING;
uint32_t detectedBaud = 0;
uint32_t stateChangeTime = 0;
bool needRestart = false;
uint32_t lastDataTime = 0;
const uint32_t DATA_TIMEOUT = 5000; // 5 seconds

// Forward declarations
void changeState(SystemState newState);
void handleWaitingState();
void handleAnalyzingState();
void handleFoundSpeedState();
void handleRestartNeededState();
void handleTestingState();
void handleRunningState();
void handleBridgeModeState();
void handleMenuState();
void logDataToSD(const char* data);

static uint32_t millis() {
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

void setup_hardware() {
    ESP_LOGI(TAG, "\n\n=== USB-TTL SNIFFER STARTING ===");
    
    // Note: UART_NUM_0 is already initialized by ESP-IDF as console
    // We'll use printf() for menu output instead of direct UART writes
    
    // Detect hardware modules
    ESP_LOGI(TAG, "Scanning for hardware modules...");
    HardwareConfig hwConfig = hwDetector.scanAll();
    
    // Initialize display only if we want to try (can be disabled for testing)
    // Set to false to skip display initialization entirely
    bool tryDisplay = false;  // Set to true when display hardware is connected
    bool displayPresent = false;
    
    if (tryDisplay) {
        displayPresent = display.begin();
        if (!displayPresent) {
            ESP_LOGW(TAG, "Display not detected — continuing without display.");
        }
        if (displayPresent) {
            display.setState(STATE_BOOTING);
            display.setStatus("Initializing components...");
            display.update();
        }
    } else {
        ESP_LOGI(TAG, "Display initialization skipped (not connected)");
    }
    
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Initialize SD card
    if (logger.begin()) {
        if (displayPresent) display.setStatus("SD card OK");
    } else {
        if (displayPresent) display.setStatus("SD card FAILED");
    }
    if (displayPresent) display.update();
    
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Log startup
    logger.log("System boot");
    logger.log("Components initialized");
    
    // Initialize baud detector
    baudDetector.begin();
    if (displayPresent) {
        display.setStatus("Initializing Bluetooth...");
        display.update();
    }
    
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Initialize Bluetooth
    bluetooth.begin("SmvIT USB-TTL Bridge");
    if (displayPresent) {
        display.setStatus("Ready");
        display.update();
    }
    
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    // Configure menu system with hardware info
    menu.setHardwareDetected(hwConfig.displayPresent, 
                            hwConfig.sdCardPresent, 
                            hwConfig.accelerometerPresent);
    
    // Link display and logger to bridge mode
    bridge.setDisplay(&display);
    bridge.setLogger(&logger);
    
    // Transition to menu state
    currentState = STATE_MENU;
    stateChangeTime = millis();
    display.setState(STATE_MENU);
    display.update();
    logger.logStateChange(STATE_MENU);
    
    // Show the menu immediately
    vTaskDelay(pdMS_TO_TICKS(500));
    menu.setBaudRate(detectedBaud);
    menu.setBytesRx(bridge.getBytesRx());
    menu.setBytesTx(bridge.getBytesTx());
    menu.show();
}

void main_loop() {
    uint32_t currentTime = millis();
    
    // Update display periodically
    if (currentTime - stateChangeTime > 100) {
        display.update();
    }
    
    // State machine
    switch(currentState) {
        case STATE_WAITING:
            handleWaitingState();
            break;
            
        case STATE_ANALYZING:
            handleAnalyzingState();
            break;
            
        case STATE_FOUND_SPEED:
            handleFoundSpeedState();
            break;
            
        case STATE_RESTART_NEEDED:
            handleRestartNeededState();
            break;
            
        case STATE_TESTING:
            handleTestingState();
            break;
            
        case STATE_RUNNING:
            handleRunningState();
            break;
            
        case STATE_BRIDGE_MODE:
            handleBridgeModeState();
            break;
            
        case STATE_MENU:
            handleMenuState();
            break;
            
        case STATE_BOOTING:
            // Already handled in setup
            break;
    }
    
    // Handle Bluetooth data bridging
    bluetooth.handleLoop();
    
    vTaskDelay(pdMS_TO_TICKS(10)); // Small delay for FreeRTOS
}

void handleWaitingState() {
    // Check if data is available
    if (baudDetector.isDataAvailable()) {
        logger.log("Data detected, starting analysis");
        changeState(STATE_ANALYZING);
    }
}

void handleAnalyzingState() {
    display.setStatus("Analyzing bit timing...");
    display.update();
    
    // Try intelligent baud detection first
    ESP_LOGI(TAG, "Attempting intelligent baud detection...");
    detectedBaud = baudDetector.detectBaudRateByTiming();
    
    if (detectedBaud > 0) {
        logger.logBaudDetection(detectedBaud);
        display.setBaudRate(detectedBaud);
        changeState(STATE_FOUND_SPEED);
    } else {
        logger.log("Baud rate detection failed, returning to waiting");
        display.setStatus("Detection failed");
        display.update();
        vTaskDelay(pdMS_TO_TICKS(1000));
        changeState(STATE_WAITING);
    }
}

void handleFoundSpeedState() {
    char buf[64];
    snprintf(buf, sizeof(buf), "Speed detected: %lu bps", (unsigned long)detectedBaud);
    display.setStatus(buf);
    display.update();
    
    snprintf(buf, sizeof(buf), "Baud rate confirmed: %lu bps", (unsigned long)detectedBaud);
    logger.log(buf);
    
    // Wait a bit before entering bridge mode
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    ESP_LOGI(TAG, "Entering bridge mode at %lu baud", (unsigned long)detectedBaud);
    changeState(STATE_BRIDGE_MODE);
}

void handleRestartNeededState() {
    display.setStatus("Please restart device");
    display.update();
    
    // This state persists until user manually restarts
    // Could add auto-restart after timeout
    static uint32_t restartTime = 0;
    if (restartTime == 0) restartTime = millis();
    
    if (millis() - restartTime > 30000) { // 30 seconds
        logger.log("Auto-restarting due to timeout");
        restartTime = millis();
        changeState(STATE_WAITING);
    }
}

void handleTestingState() {
    display.setStatus("Testing communication...");
    display.update();
    
    // Initialize communication tester
    commTester.begin(detectedBaud);
    
    // Send test message and check response
    bool testResult = commTester.testCommunication();
    
    logger.logTestResult(testResult);
    
    if (testResult) {
        display.setStatus("Device responding!");
        display.update();
        logger.log("Device communication test passed");
        vTaskDelay(pdMS_TO_TICKS(2000));
        changeState(STATE_RUNNING);
    } else {
        display.setStatus("No response from device");
        display.update();
        logger.log("Device communication test failed");
        vTaskDelay(pdMS_TO_TICKS(2000));
        changeState(STATE_RESTART_NEEDED);
    }
}

void handleRunningState() {
    // Monitor RX and TX continuously
    static uint32_t lastCheck = 0;
    
    if (millis() - lastCheck > 100) {
        lastCheck = millis();
        
        // Check RX via UART
        uint8_t data[128];
        int len = uart_read_bytes(UART_NUM_1, data, sizeof(data), pdMS_TO_TICKS(10));
        
        if (len > 0) {
            // Log and display the data
            char rxData[256];
            snprintf(rxData, sizeof(rxData), "RX: ");
            for (int i = 0; i < len && i < 32; i++) {
                char hex[4];
                snprintf(hex, sizeof(hex), "%02X ", data[i]);
                strcat(rxData, hex);
            }
            logger.log(rxData);
            display.addData(rxData);
            display.update();
            lastDataTime = millis();
            
            // Forward USB data to Bluetooth if connected
            if (bluetooth.isConnected()) {
                bluetooth.write(data, len);
                ESP_LOGD(TAG, "Forwarded %d bytes from USB to Bluetooth", len);
            }
        }
        
        // Check for Bluetooth data to forward to USB
        if (bluetooth.isConnected() && bluetooth.available() > 0) {
            uint8_t btData[128];
            size_t btLen = bluetooth.readBytes(btData, sizeof(btData));
            
            if (btLen > 0) {
                // Forward Bluetooth data to USB/UART
                uart_write_bytes(UART_NUM_1, btData, btLen);
                
                // Log the Bluetooth data
                char btRxData[256];
                snprintf(btRxData, sizeof(btRxData), "BT->USB: ");
                for (int i = 0; i < btLen && i < 32; i++) {
                    char hex[4];
                    snprintf(hex, sizeof(hex), "%02X ", btData[i]);
                    strcat(btRxData, hex);
                }
                logger.log(btRxData);
                display.addData(btRxData);
                ESP_LOGD(TAG, "Forwarded %d bytes from Bluetooth to USB", btLen);
            }
        }
        
        // Check if we should send test message periodically
        static uint32_t lastTest = 0;
        if (millis() - lastTest > 10000) { // Every 10 seconds
            commTester.sendTestMessage();
            lastTest = millis();
        }
        
        // Check for timeout
        if (millis() - lastDataTime > DATA_TIMEOUT) {
            logger.log("Data timeout, returning to waiting");
            changeState(STATE_WAITING);
        }
    }
    
    display.update();
}

void handleBridgeModeState() {
    // Enter bridge mode on first call
    if (!bridge.isActive()) {
        bridge.enter(detectedBaud);
        display.setStatus("Bridge Mode Active");
        display.update();
    }
    
    // Handle transparent data passthrough
    bridge.handleData();
    
    // Check if bridge mode was exited (escape sequence detected)
    if (!bridge.isActive()) {
        ESP_LOGI(TAG, "Bridge mode exited, returning to menu");
        // Update stats before showing menu
        menu.setBytesRx(bridge.getBytesRx());
        menu.setBytesTx(bridge.getBytesTx());
        changeState(STATE_MENU);
    }
}

void handleMenuState() {
    static bool menuShown = false;
    static SystemState lastState = STATE_BOOTING;
    
    // Reset menu flag if we just entered this state from another state
    if (lastState != STATE_MENU) {
        menuShown = false;
        lastState = STATE_MENU;
    }
    
    // Show menu on first entry
    if (!menuShown) {
        // Update menu with current stats
        menu.setBaudRate(detectedBaud);
        menu.setBytesRx(bridge.getBytesRx());
        menu.setBytesTx(bridge.getBytesTx());
        menu.show();
        menuShown = true;
    }
    
    // Check for user input
    int c = getchar();
    
    if (c != EOF) {
        // Echo the character
        printf("%c", (char)c);
        
        // Handle the input
        menu.handleInput((char)c);
        
        // Check for commands that change state
        char cmd = menu.getLastCommand();
        if (cmd != 0) {
            switch (cmd) {
                case 'D':  // Detect baud rate
                    ESP_LOGI(TAG, "Starting baud detection...");
                    printf("\r\n\r\n");
                    menuShown = false;
                    changeState(STATE_ANALYZING);
                    break;
                    
                case 'B':  // Enter bridge mode
                    if (detectedBaud > 0) {
                        ESP_LOGI(TAG, "Entering bridge mode...");
                        printf("\r\n\r\n");
                        menuShown = false;
                        changeState(STATE_BRIDGE_MODE);
                    } else {
                        printf("\r\n[ERROR] No baud rate detected. Run detection first.\r\n");
                        menuShown = false;  // Redraw menu
                    }
                    break;
                        
                case 'R':  // Reset detection
                    ESP_LOGI(TAG, "Resetting...");
                    printf("\r\n\r\n");
                    detectedBaud = 0;
                    bridge.resetStats();
                    menuShown = false;
                    changeState(STATE_WAITING);
                    break;
                    
                case '1':  // Manual baud rates
                case '2':
                case '3':
                case '4':
                case '5':
                    {
                        uint32_t baudRates[] = {9600, 19200, 38400, 57600, 115200};
                        int idx = cmd - '1';
                        if (idx >= 0 && idx < 5) {
                            detectedBaud = baudRates[idx];
                            printf("\r\nBaud rate set to %lu\r\n", (unsigned long)detectedBaud);
                            ESP_LOGI(TAG, "Manual baud rate: %lu", (unsigned long)detectedBaud);
                            vTaskDelay(pdMS_TO_TICKS(1000));
                            menuShown = false;
                        }
                    }
                    break;
            }
            
            menu.clearCommand();
        }
    }
}

void changeState(SystemState newState) {
    if (currentState != newState) {
        currentState = newState;
        stateChangeTime = millis();
        display.setState(newState);
        logger.logStateChange(newState);
    }
}

void logDataToSD(const char* data) {
    logger.log(data);
}

extern "C" void app_main(void) {
    setup_hardware();
    
    while (1) {
        main_loop();
    }
}

