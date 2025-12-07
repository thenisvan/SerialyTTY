#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include <cstdint>
#include "config.h"

// Menu states
enum MenuScreen {
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_STATS,
    MENU_HARDWARE,
    MENU_HELP
};

class MenuSystem {
public:
    MenuSystem();
    
    // Menu control
    void show();
    void update();
    void handleInput(char c);
    
    // Menu navigation
    void showMainMenu();
    void showSettingsMenu();
    void showStatsMenu();
    void showHardwareInfo();
    void showHelpMenu();
    
    // Set context information
    void setBaudRate(uint32_t baud) { currentBaud = baud; }
    void setBytesRx(uint32_t bytes) { bytesRx = bytes; }
    void setBytesTx(uint32_t bytes) { bytesTx = bytes; }
    void setHardwareDetected(bool display, bool sd, bool accel) {
        hasDisplay = display;
        hasSD = sd;
        hasAccel = accel;
    }
    
    // Get user choice
    char getLastCommand() const { return lastCommand; }
    void clearCommand() { lastCommand = 0; }

private:
    MenuScreen currentScreen;
    char lastCommand;
    
    // Context data
    uint32_t currentBaud;
    uint32_t bytesRx;
    uint32_t bytesTx;
    bool hasDisplay;
    bool hasSD;
    bool hasAccel;
    
    // Helper methods
    void printLine(const char* line);
    void printHeader(const char* title);
    void printSeparator();
    void clearScreen();
};

#endif // MENU_SYSTEM_H
