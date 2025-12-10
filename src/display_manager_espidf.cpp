#include "display_manager.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "DISPLAY";

DisplayManager::DisplayManager() : tft(nullptr), rxBytes(0), txBytes(0) {
    currentState = STATE_BOOTING;
    statusText[0] = '\0';
    baudText[0] = '\0';
    dataText[0] = '\0';
    needsUpdate = true;
    present = false;
}

DisplayManager::~DisplayManager() {
    if (tft != nullptr) {
        delete tft;
    }
}

bool DisplayManager::begin() {
    ESP_LOGI(TAG, "Initializing display manager...");
    
    tft = new TFTDriver();
    if (tft == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate TFT driver");
        present = false;
        return false;
    }
    
    if (!tft->begin()) {
        ESP_LOGI(TAG, "TFT display not detected - running without display");
        delete tft;
        tft = nullptr;
        present = false;
        return false;
    }
    
    present = true;
    ESP_LOGI(TAG, "Display initialized successfully");
    
    // Draw initial boot screen
    drawBootScreen();
    
    return true;
}

void DisplayManager::clear() {
    if (!present || tft == nullptr) return;
    tft->fillScreen(TFT_BLACK);
}

void DisplayManager::update() {
    if (!present || tft == nullptr) return;
    if (!needsUpdate) return;
    
    // Update display based on current state
    switch(currentState) {
        case STATE_BOOTING:
            drawBootScreen();
            break;
        case STATE_MENU:
            drawMenuScreen();
            break;
        case STATE_ANALYZING:
            drawAnalyzingScreen();
            break;
        case STATE_BRIDGE_MODE:
            drawBridgeScreen();
            break;
        default:
            drawStatusBar();
            break;
    }
    
    needsUpdate = false;
}

void DisplayManager::setState(SystemState state) {
    if (!present) return;
    if (currentState != state) {
        currentState = state;
        needsUpdate = true;
        ESP_LOGI(TAG, "State changed to: %s", getStateName(state));
    }
}

void DisplayManager::setBaudRate(uint32_t baud) {
    if (!present) return;
    snprintf(baudText, sizeof(baudText), "%lu bps", (unsigned long)baud);
    needsUpdate = true;
}

void DisplayManager::setDataStats(uint32_t rx, uint32_t tx) {
    if (!present) return;
    rxBytes = rx;
    txBytes = tx;
    needsUpdate = true;
}

void DisplayManager::addData(const char* data) {
    if (!present) return;
    strncpy(dataText, data, sizeof(dataText) - 1);
    dataText[sizeof(dataText) - 1] = '\0';
    needsUpdate = true;
}

void DisplayManager::setStatus(const char* status) {
    if (!present) return;
    strncpy(statusText, status, sizeof(statusText) - 1);
    statusText[sizeof(statusText) - 1] = '\0';
    needsUpdate = true;
    ESP_LOGI(TAG, "Status: %s", status);
}

bool DisplayManager::isPresent() {
    return present;
}

void DisplayManager::drawHeader(const char* title) {
    if (!tft) return;
    
    // Draw header bar
    tft->fillRect(0, 0, TFT_WIDTH, 30, TFT_BLUE);
    tft->drawStringCentered(11, title, TFT_WHITE, TFT_BLUE);
}

void DisplayManager::drawStatusBar() {
    if (!tft) return;
    
    // Draw status bar at bottom
    tft->fillRect(0, TFT_HEIGHT - 20, TFT_WIDTH, 20, TFT_DARKGRAY);
    
    char buf[64];
    snprintf(buf, sizeof(buf), "RX:%lu TX:%lu", (unsigned long)rxBytes, (unsigned long)txBytes);
    tft->drawString(5, TFT_HEIGHT - 15, buf, TFT_WHITE, TFT_DARKGRAY);
}

void DisplayManager::drawBootScreen() {
    if (!tft) return;
    
    tft->fillScreen(TFT_BLACK);
    drawHeader("SmvIT v1.0");
    
    tft->drawStringCentered(100, "ESP32-C6", TFT_CYAN, TFT_BLACK);
    tft->drawStringCentered(120, "USB-TTL Bridge", TFT_GREEN, TFT_BLACK);
    tft->drawStringCentered(160, "Booting...", TFT_YELLOW, TFT_BLACK);
}

void DisplayManager::drawMenuScreen() {
    if (!tft) return;
    
    tft->fillScreen(TFT_BLACK);
    drawHeader("Main Menu");
    
    int y = 50;
    tft->drawString(20, y, "D - Detect Hardware", TFT_WHITE, TFT_BLACK); y += 20;
    tft->drawString(20, y, "B - Bridge Mode", TFT_WHITE, TFT_BLACK); y += 20;
    tft->drawString(20, y, "S - Settings", TFT_WHITE, TFT_BLACK); y += 20;
    tft->drawString(20, y, "I - Info", TFT_WHITE, TFT_BLACK); y += 20;
    tft->drawString(20, y, "H - Help", TFT_WHITE, TFT_BLACK); y += 20;
    
    y += 30;
    if (statusText[0] != '\0') {
        tft->drawString(20, y, statusText, TFT_CYAN, TFT_BLACK);
    }
    
    drawStatusBar();
}

void DisplayManager::drawAnalyzingScreen() {
    if (!tft) return;
    
    tft->fillScreen(TFT_BLACK);
    drawHeader("Baud Detection");
    
    tft->drawStringCentered(100, "Analyzing signal...", TFT_YELLOW, TFT_BLACK);
    tft->drawStringCentered(140, "Send data from device", TFT_GRAY, TFT_BLACK);
    
    if (statusText[0] != '\0') {
        tft->drawStringCentered(180, statusText, TFT_CYAN, TFT_BLACK);
    }
    
    drawStatusBar();
}

void DisplayManager::drawBridgeScreen() {
    if (!tft) return;
    
    tft->fillScreen(TFT_BLACK);
    drawHeader("Bridge Mode");
    
    int y = 50;
    tft->drawString(20, y, "Status: ACTIVE", TFT_GREEN, TFT_BLACK); y += 30;
    
    if (baudText[0] != '\0') {
        tft->drawString(20, y, "Baud: ", TFT_WHITE, TFT_BLACK);
        tft->drawString(80, y, baudText, TFT_CYAN, TFT_BLACK);
        y += 30;
    }
    
    char buf[64];
    snprintf(buf, sizeof(buf), "RX: %lu bytes", (unsigned long)rxBytes);
    tft->drawString(20, y, buf, TFT_WHITE, TFT_BLACK); y += 20;
    
    snprintf(buf, sizeof(buf), "TX: %lu bytes", (unsigned long)txBytes);
    tft->drawString(20, y, buf, TFT_WHITE, TFT_BLACK); y += 20;
    
    y += 30;
    tft->drawStringCentered(y, "Type ~~~ to exit", TFT_YELLOW, TFT_BLACK);
    
    if (dataText[0] != '\0') {
        y += 30;
        tft->drawString(20, y, dataText, TFT_GRAY, TFT_BLACK);
    }
}

const char* DisplayManager::getStateName(SystemState state) {
    switch(state) {
        case STATE_BOOTING: return "Boot";
        case STATE_MENU: return "Menu";
        case STATE_ANALYZING: return "Analyze";
        case STATE_BRIDGE_MODE: return "Bridge";
        case STATE_WAITING: return "Waiting";
        case STATE_FOUND_SPEED: return "Found Speed";
        case STATE_RESTART_NEEDED: return "Restart Needed";
        case STATE_TESTING: return "Testing";
        case STATE_RUNNING: return "Running";
        default: return "Unknown";
    }
}
