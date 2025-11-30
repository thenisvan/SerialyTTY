#include "display_manager.h"

DisplayManager::DisplayManager() {
    currentState = STATE_BOOTING;
    statusText = "";
    baudText = "";
    dataText = "";
    needsUpdate = true;
    present = false;
}

bool DisplayManager::begin() {
    // Try to initialize the TFT. If display is not present, make all
    // public methods no-ops so the core keeps running.
    tft.init();
    tft.setRotation(1); // Landscape

    // Try to detect display by reading ID (common for many TFT drivers).
    uint16_t id = 0;
#ifdef TFT_READ_ID
    id = tft.readID();
#else
    // If readID is not defined, assume display may be present after init.
    // We'll treat non-zero width/height as heuristic if available.
    // Default to present=true to avoid false negative on some setups.
    id = 1;
#endif

    if (id == 0 || id == 0xFFFF) {
        present = false;
        return false;
    }

    present = true;
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    return true;
}

void DisplayManager::clear() {
    if (!present) return;
    tft.fillScreen(TFT_BLACK);
}

void DisplayManager::update() {
    if (!present) return;
    if (needsUpdate) {
        clear();
        
        switch(currentState) {
            case STATE_BOOTING:
                drawBootScreen();
                break;
            case STATE_WAITING:
                drawWaitingScreen();
                break;
            case STATE_ANALYZING:
                drawAnalyzingScreen();
                break;
            case STATE_FOUND_SPEED:
                drawFoundSpeedScreen();
                break;
            case STATE_RESTART_NEEDED:
                drawRestartNeededScreen();
                break;
            case STATE_TESTING:
                drawTestingScreen();
                break;
            case STATE_RUNNING:
                drawRunningScreen();
                break;
        }
        
        needsUpdate = false;
    }
}

void DisplayManager::setState(SystemState state) {
    if (!present) return;
    if (currentState != state) {
        currentState = state;
        needsUpdate = true;
    }
}

void DisplayManager::setBaudRate(uint32_t baud) {
    if (!present) return;
    baudText = String(baud) + " bps";
    needsUpdate = true;
}

void DisplayManager::addData(String data) {
    if (!present) return;
    dataText = data;
    needsUpdate = true;
}

void DisplayManager::setStatus(String status) {
    if (!present) return;
    statusText = status;
    needsUpdate = true;
}

bool DisplayManager::isPresent() {
    return present;
}

void DisplayManager::drawBootScreen() {
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("USB-TTL SNIFFER");
    tft.setCursor(10, 40);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.print("Initializing...");
    tft.setCursor(10, 60);
    tft.print("System: Booting");
}

void DisplayManager::drawWaitingScreen() {
    tft.setTextSize(2);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("WAITING");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.print("Waiting for communication...");
    tft.setCursor(10, 60);
    tft.print("No data detected");
}

void DisplayManager::drawAnalyzingScreen() {
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("ANALYZING");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.print("Detecting baud rate...");
    tft.setCursor(10, 60);
    tft.print("State: " + String(getStateName(currentState).c_str()));
}

void DisplayManager::drawFoundSpeedScreen() {
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("SPEED FOUND");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.print("Baud Rate: " + baudText);
    tft.setCursor(10, 60);
    tft.print(statusText);
}

void DisplayManager::drawRestartNeededScreen() {
    tft.setTextSize(2);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("RESTART");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.print("Please restart device");
    tft.setCursor(10, 60);
    tft.print("Found: " + baudText);
}

void DisplayManager::drawTestingScreen() {
    tft.setTextSize(2);
    tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("TESTING");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.print("Testing communication...");
    tft.setCursor(10, 60);
    tft.print("Sending test data");
    tft.setCursor(10, 80);
    tft.print("Baud: " + baudText);
}

void DisplayManager::drawRunningScreen() {
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("RUNNING");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.print("Baud: " + baudText);
    tft.setCursor(10, 60);
    tft.print("RX/TX active");
    if (dataText.length() > 0) {
        tft.setCursor(10, 80);
        tft.print(dataText);
    }
}

String DisplayManager::getStateName(SystemState state) {
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

