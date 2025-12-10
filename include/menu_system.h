#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include <cstdint>
#include "config.h"

// Menu states - hierarchical structure
enum MenuScreen {
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_SETTINGS_UART,
    MENU_SETTINGS_BLUETOOTH,
    MENU_SETTINGS_DISPLAY,
    MENU_SETTINGS_LOGGING,
    MENU_SETTINGS_SYSTEM,
    MENU_STATS,
    MENU_HARDWARE,
    MENU_HELP
};

// Menu navigation history stack
#define MAX_MENU_HISTORY 10

// Forward declaration
class BluetoothManager;

class MenuSystem {
public:
    MenuSystem();
    
    // Set Bluetooth manager reference (must be called before using BT features)
    void setBluetoothManager(BluetoothManager* bt) { bluetoothMgr = bt; }
    
    // Menu control
    void show();
    void update();
    void handleInput(char c);
    
    // Menu navigation
    void showMainMenu();
    void showSettingsMenu();
    void showSettingsUART();
    void showSettingsBluetooth();
    void showSettingsDisplay();
    void showSettingsLogging();
    void showSettingsSystem();
    void showStatsMenu();
    void showHardwareInfo();
    void showHelpMenu();
    
    // Navigation helpers
    void navigateTo(MenuScreen screen);
    void navigateBack();
    MenuScreen getCurrentScreen() const { return currentScreen; }
    
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
    bool isEditingMode() const { return editingMode; }

private:
    MenuScreen currentScreen;
    MenuScreen menuHistory[MAX_MENU_HISTORY];
    int historyIndex;
    char lastCommand;
    
    // Input buffer for settings editing
    char inputBuffer[64];
    int inputPosition;
    bool editingMode;
    
    // Context data
    uint32_t currentBaud;
    uint32_t bytesRx;
    uint32_t bytesTx;
    bool hasDisplay;
    bool hasSD;
    bool hasAccel;
    
    // Reference to Bluetooth manager for runtime control
    BluetoothManager* bluetoothMgr;
    
    // Helper methods
    void printLine(const char* line);
    void printHeader(const char* title);
    void printSeparator();
    void clearScreen();
    void printBreadcrumb();
    
    // Settings editing helpers
    void startEdit(const char* prompt);
    void handleEditInput(char c);
    bool finishEdit();
    void cancelEdit();
    
    // Settings display helpers
    void printBoolSetting(const char* label, bool value);
    void printStringSetting(const char* label, const char* value);
    void printNumberSetting(const char* label, uint32_t value, const char* unit = "");
    void printEnumSetting(const char* label, uint8_t value, const char** options, int optCount);
    
    // Bluetooth settings handlers
    void handleBluetoothToggle();
    void handleBluetoothNameEdit();
    void handleBluetoothAutoAdvertiseToggle();
    void handleBluetoothPairingToggle();
    void handleBluetoothPinEdit();
    void handleBluetoothTxPowerEdit();
    
    // UART settings handlers
    void handleUARTBaudEdit();
    void handleUARTAutoDetectToggle();
    void handleUARTParityEdit();
    void handleUARTStopBitsEdit();
    void handleUARTFlowControlEdit();
};

#endif // MENU_SYSTEM_H
