#include "display_manager.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "DISPLAY";

DisplayManager::DisplayManager() {
    currentState = STATE_BOOTING;
    statusText[0] = '\0';
    baudText[0] = '\0';
    dataText[0] = '\0';
    needsUpdate = true;
    present = false;
}

bool DisplayManager::begin() {
    // TODO: Initialize SPI display driver
    // For now, return false (no display) until we add ESP-IDF display driver
    ESP_LOGW(TAG, "Display driver not yet implemented for ESP-IDF");
    present = false;
    return false;
}

void DisplayManager::clear() {
    if (!present) return;
    // TODO: Clear display
}

void DisplayManager::update() {
    if (!present) return;
    if (!needsUpdate) return;
    
    // TODO: Update display based on current state
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

void DisplayManager::drawBootScreen() {
    // TODO: Implement
}

void DisplayManager::drawWaitingScreen() {
    // TODO: Implement
}

void DisplayManager::drawAnalyzingScreen() {
    // TODO: Implement
}

void DisplayManager::drawFoundSpeedScreen() {
    // TODO: Implement
}

void DisplayManager::drawRestartNeededScreen() {
    // TODO: Implement
}

void DisplayManager::drawTestingScreen() {
    // TODO: Implement
}

void DisplayManager::drawRunningScreen() {
    // TODO: Implement
}

const char* DisplayManager::getStateName(SystemState state) {
    switch(state) {
        case STATE_BOOTING: return "Boot";
        case STATE_WAITING: return "Wait";
        case STATE_ANALYZING: return "Analyze";
        case STATE_FOUND_SPEED: return "Found";
        case STATE_RESTART_NEEDED: return "Restart";
        case STATE_TESTING: return "Test";
        case STATE_RUNNING: return "Run";
        default: return "Unknown";
    }
}
