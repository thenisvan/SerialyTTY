#include "menu_system.h"
#include "config_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

static const char *TAG = "MENU";

// ANSI escape codes for terminal formatting
#define ANSI_CLEAR "\033[2J\033[H"
#define ANSI_BOLD "\033[1m"
#define ANSI_RESET "\033[0m"
#define ANSI_GREEN "\033[32m"
#define ANSI_CYAN "\033[36m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_RED "\033[31m"
#define ANSI_MAGENTA "\033[35m"

MenuSystem::MenuSystem() :
    currentScreen(MENU_MAIN),
    historyIndex(0),
    lastCommand(0),
    inputPosition(0),
    editingMode(false),
    currentBaud(0),
    bytesRx(0),
    bytesTx(0),
    hasDisplay(false),
    hasSD(false),
    hasAccel(false)
{
    memset(menuHistory, 0, sizeof(menuHistory));
    memset(inputBuffer, 0, sizeof(inputBuffer));
}

void MenuSystem::printLine(const char* line) {
    printf("%s\r\n", line);
}

void MenuSystem::printHeader(const char* title) {
    char header[256];
    snprintf(header, sizeof(header), 
             ANSI_BOLD ANSI_CYAN "╔════════════════════════════════════════╗" ANSI_RESET);
    printLine(header);
    
    snprintf(header, sizeof(header), 
             ANSI_BOLD ANSI_CYAN "║" ANSI_RESET " %-38s " ANSI_BOLD ANSI_CYAN "║" ANSI_RESET, title);
    printLine(header);
    
    snprintf(header, sizeof(header), 
             ANSI_BOLD ANSI_CYAN "╚════════════════════════════════════════╝" ANSI_RESET);
    printLine(header);
}

void MenuSystem::printSeparator() {
    printLine(ANSI_CYAN "────────────────────────────────────────" ANSI_RESET);
}

void MenuSystem::clearScreen() {
    printf(ANSI_CLEAR);
}

void MenuSystem::printBreadcrumb() {
    const char* breadcrumb = "Main";
    if (currentScreen == MENU_SETTINGS) breadcrumb = "Main > Settings";
    else if (currentScreen == MENU_SETTINGS_UART) breadcrumb = "Main > Settings > UART";
    else if (currentScreen == MENU_SETTINGS_BLUETOOTH) breadcrumb = "Main > Settings > Bluetooth";
    else if (currentScreen == MENU_SETTINGS_DISPLAY) breadcrumb = "Main > Settings > Display";
    else if (currentScreen == MENU_SETTINGS_LOGGING) breadcrumb = "Main > Settings > Logging";
    else if (currentScreen == MENU_SETTINGS_SYSTEM) breadcrumb = "Main > Settings > System";
    else if (currentScreen == MENU_STATS) breadcrumb = "Main > Statistics";
    else if (currentScreen == MENU_HARDWARE) breadcrumb = "Main > Hardware";
    else if (currentScreen == MENU_HELP) breadcrumb = "Main > Help";
    
    char line[128];
    snprintf(line, sizeof(line), ANSI_CYAN "📍 %s" ANSI_RESET, breadcrumb);
    printLine(line);
    printLine("");
}

void MenuSystem::printBoolSetting(const char* label, bool value) {
    char line[128];
    const char* status = value ? ANSI_GREEN "ON" : ANSI_RED "OFF";
    snprintf(line, sizeof(line), "  %s: [%s" ANSI_RESET "]", label, status);
    printLine(line);
}

void MenuSystem::printStringSetting(const char* label, const char* value) {
    char line[128];
    snprintf(line, sizeof(line), "  %s: " ANSI_YELLOW "\"%s\"" ANSI_RESET, label, value);
    printLine(line);
}

void MenuSystem::printNumberSetting(const char* label, uint32_t value, const char* unit) {
    char line[128];
    snprintf(line, sizeof(line), "  %s: " ANSI_CYAN "%lu" ANSI_RESET " %s", label, (unsigned long)value, unit);
    printLine(line);
}

void MenuSystem::navigateTo(MenuScreen screen) {
    if (historyIndex < MAX_MENU_HISTORY - 1) {
        menuHistory[historyIndex++] = currentScreen;
    }
    currentScreen = screen;
}

void MenuSystem::navigateBack() {
    if (historyIndex > 0) {
        currentScreen = menuHistory[--historyIndex];
    } else {
        currentScreen = MENU_MAIN;
    }
}

// ============================================================================
// Main Menu
// ============================================================================

void MenuSystem::showMainMenu() {
    clearScreen();
    printHeader("SmvIT USB-TTL Bridge - Main Menu");
    printLine("");
    
    char line[128];
    
    // Show current status
    printLine(ANSI_BOLD "Current Status:" ANSI_RESET);
    if (currentBaud > 0) {
        snprintf(line, sizeof(line), "  Baud Rate: " ANSI_GREEN "%lu bps" ANSI_RESET, 
                 (unsigned long)currentBaud);
        printLine(line);
        
        snprintf(line, sizeof(line), "  RX: " ANSI_YELLOW "%lu" ANSI_RESET " bytes  |  TX: " ANSI_YELLOW "%lu" ANSI_RESET " bytes",
                 (unsigned long)bytesRx, (unsigned long)bytesTx);
        printLine(line);
    } else {
        printLine("  " ANSI_YELLOW "No connection detected" ANSI_RESET);
    }
    
    printLine("");
    printSeparator();
    printLine("");
    
    // Menu options
    printLine(ANSI_BOLD "Available Commands:" ANSI_RESET);
    printLine("");
    printLine("  " ANSI_GREEN "[B]" ANSI_RESET " Bridge Mode    - Transparent serial passthrough");
    printLine("  " ANSI_GREEN "[D]" ANSI_RESET " Detect         - Scan for baud rate");
    printLine("  " ANSI_GREEN "[S]" ANSI_RESET " Settings       - Configuration menu");
    printLine("  " ANSI_GREEN "[I]" ANSI_RESET " Statistics     - View detailed stats");
    printLine("  " ANSI_GREEN "[H]" ANSI_RESET " Hardware       - Show detected hardware");
    printLine("  " ANSI_GREEN "[?]" ANSI_RESET " Help           - Show help information");
    printLine("  " ANSI_GREEN "[R]" ANSI_RESET " Reset          - Restart detection");
    printLine("");
    printSeparator();
    printLine("");
    printLine("Enter command: ");
}

// ============================================================================
// Settings Menu (Root)
// ============================================================================

void MenuSystem::showSettingsMenu() {
    clearScreen();
    printHeader("Settings");
    printBreadcrumb();
    
    ConfigManager& cfg = ConfigManager::getInstance();
    const DeviceConfig& config = cfg.getConfig();
    
    printLine(ANSI_BOLD "Configuration Categories:" ANSI_RESET);
    printLine("");
    printLine("  " ANSI_GREEN "[1]" ANSI_RESET " UART Settings       - Baud rate, parity, stop bits");
    printLine("  " ANSI_GREEN "[2]" ANSI_RESET " Bluetooth Settings  - BLE configuration");
    printLine("  " ANSI_GREEN "[3]" ANSI_RESET " Display Settings    - Brightness, timeout");
    printLine("  " ANSI_GREEN "[4]" ANSI_RESET " Logging Settings    - Log level, file size");
    printLine("  " ANSI_GREEN "[5]" ANSI_RESET " System Settings     - Device name, timezone");
    printLine("");
    
    printLine(ANSI_BOLD "Quick Status:" ANSI_RESET);
    char line[128];
    snprintf(line, sizeof(line), "  UART: %lu bps | BLE: %s | Display: %s | Logging: %s",
             (unsigned long)config.uart.baudRate,
             config.bluetooth.enabled ? "ON" : "OFF",
             config.display.enabled ? "ON" : "OFF",
             config.logging.enabled ? "ON" : "OFF");
    printLine(line);
    printLine("");
    
    printSeparator();
    printLine("");
    printLine("  " ANSI_YELLOW "[0]" ANSI_RESET " Factory Reset");
    printLine("  " ANSI_CYAN "[M]" ANSI_RESET " Back to Main Menu");
    printLine("");
    printLine("Enter option: ");
}

// ============================================================================
// Bluetooth Settings Submenu
// ============================================================================

void MenuSystem::showSettingsBluetooth() {
    clearScreen();
    printHeader("Bluetooth Settings");
    printBreadcrumb();
    
    ConfigManager& cfg = ConfigManager::getInstance();
    const DeviceConfig& config = cfg.getConfig();
    
    printLine(ANSI_BOLD "Current Bluetooth Configuration:" ANSI_RESET);
    printLine("");
    
    // Status
    char line[128];
    const char* statusColor = config.bluetooth.enabled ? ANSI_GREEN : ANSI_RED;
    const char* statusText = config.bluetooth.enabled ? "ENABLED" : "DISABLED";
    snprintf(line, sizeof(line), "  Status: %s%s" ANSI_RESET, statusColor, statusText);
    printLine(line);
    printLine("");
    
    // Settings
    printStringSetting("Device Name", config.bluetooth.deviceName);
    printBoolSetting("Auto-Advertise on Boot", config.bluetooth.autoAdvertise);
    printBoolSetting("Require Pairing", config.bluetooth.requirePairing);
    
    if (config.bluetooth.requirePairing) {
        printStringSetting("PIN Code", config.bluetooth.pinCode);
    }
    
    printNumberSetting("Advertising Interval", config.bluetooth.advertisingInterval, "ms");
    
    snprintf(line, sizeof(line), "  TX Power: " ANSI_CYAN "%d" ANSI_RESET " dBm", config.bluetooth.txPower);
    printLine(line);
    
    printLine("");
    printSeparator();
    printLine("");
    
    printLine(ANSI_BOLD "Options:" ANSI_RESET);
    printLine("");
    printLine("  " ANSI_GREEN "[E]" ANSI_RESET " Enable/Disable Bluetooth");
    printLine("  " ANSI_GREEN "[N]" ANSI_RESET " Change Device Name");
    printLine("  " ANSI_GREEN "[A]" ANSI_RESET " Toggle Auto-Advertise");
    printLine("  " ANSI_GREEN "[P]" ANSI_RESET " Toggle Pairing Requirement");
    printLine("  " ANSI_GREEN "[C]" ANSI_RESET " Change PIN Code");
    printLine("  " ANSI_GREEN "[I]" ANSI_RESET " Set Advertising Interval");
    printLine("  " ANSI_GREEN "[T]" ANSI_RESET " Set TX Power");
    printLine("");
    printLine("  " ANSI_CYAN "[M]" ANSI_RESET " Back to Settings Menu");
    printLine("");
    printLine("Enter option: ");
}

// ============================================================================
// UART Settings Submenu
// ============================================================================

void MenuSystem::showSettingsUART() {
    clearScreen();
    printHeader("UART Settings");
    printBreadcrumb();
    
    ConfigManager& cfg = ConfigManager::getInstance();
    const DeviceConfig& config = cfg.getConfig();
    
    printLine(ANSI_BOLD "Current UART Configuration:" ANSI_RESET);
    printLine("");
    
    printNumberSetting("Baud Rate", config.uart.baudRate, "bps");
    printBoolSetting("Auto-Detect", config.uart.autoDetect);
    
    const char* parityNames[] = {"None", "Even", "Odd"};
    char line[128];
    snprintf(line, sizeof(line), "  Parity: " ANSI_CYAN "%s" ANSI_RESET, parityNames[config.uart.parity]);
    printLine(line);
    
    printNumberSetting("Stop Bits", config.uart.stopBits, "");
    
    const char* flowNames[] = {"None", "RTS/CTS", "XON/XOFF"};
    snprintf(line, sizeof(line), "  Flow Control: " ANSI_CYAN "%s" ANSI_RESET, flowNames[config.uart.flowControl]);
    printLine(line);
    
    printLine("");
    printSeparator();
    printLine("");
    
    printLine(ANSI_BOLD "Options:" ANSI_RESET);
    printLine("");
    printLine("  " ANSI_GREEN "[B]" ANSI_RESET " Set Baud Rate");
    printLine("  " ANSI_GREEN "[A]" ANSI_RESET " Toggle Auto-Detect");
    printLine("  " ANSI_GREEN "[P]" ANSI_RESET " Set Parity");
    printLine("  " ANSI_GREEN "[S]" ANSI_RESET " Set Stop Bits");
    printLine("  " ANSI_GREEN "[F]" ANSI_RESET " Set Flow Control");
    printLine("");
    printLine("  " ANSI_CYAN "[M]" ANSI_RESET " Back to Settings Menu");
    printLine("");
    printLine("Enter option: ");
}

// ============================================================================
// Display Settings Submenu
// ============================================================================

void MenuSystem::showSettingsDisplay() {
    clearScreen();
    printHeader("Display Settings");
    printBreadcrumb();
    
    ConfigManager& cfg = ConfigManager::getInstance();
    const DeviceConfig& config = cfg.getConfig();
    
    printLine(ANSI_BOLD "Current Display Configuration:" ANSI_RESET);
    printLine("");
    
    printBoolSetting("Display Enabled", config.display.enabled);
    printNumberSetting("Brightness", config.display.brightness, "(0-255)");
    printNumberSetting("Timeout", config.display.timeout, "seconds");
    printNumberSetting("Rotation", config.display.rotation, "(0-3)");
    printBoolSetting("Auto-Off", config.display.autoOff);
    
    printLine("");
    printSeparator();
    printLine("");
    
    printLine(ANSI_BOLD "Options:" ANSI_RESET);
    printLine("");
    printLine("  " ANSI_GREEN "[E]" ANSI_RESET " Enable/Disable Display");
    printLine("  " ANSI_GREEN "[B]" ANSI_RESET " Set Brightness");
    printLine("  " ANSI_GREEN "[T]" ANSI_RESET " Set Timeout");
    printLine("  " ANSI_GREEN "[R]" ANSI_RESET " Set Rotation");
    printLine("  " ANSI_GREEN "[A]" ANSI_RESET " Toggle Auto-Off");
    printLine("");
    printLine("  " ANSI_CYAN "[M]" ANSI_RESET " Back to Settings Menu");
    printLine("");
    printLine("Enter option: ");
}

// ============================================================================
// Logging Settings Submenu
// ============================================================================

void MenuSystem::showSettingsLogging() {
    clearScreen();
    printHeader("Logging Settings");
    printBreadcrumb();
    
    ConfigManager& cfg = ConfigManager::getInstance();
    const DeviceConfig& config = cfg.getConfig();
    
    printLine(ANSI_BOLD "Current Logging Configuration:" ANSI_RESET);
    printLine("");
    
    printBoolSetting("Logging Enabled", config.logging.enabled);
    
    const char* levelNames[] = {"None", "Error", "Warn", "Info", "Debug", "Verbose"};
    char line[128];
    snprintf(line, sizeof(line), "  Log Level: " ANSI_CYAN "%s" ANSI_RESET, levelNames[config.logging.logLevel]);
    printLine(line);
    
    printNumberSetting("Max File Size", config.logging.maxFileSizeKB, "KB");
    printBoolSetting("Auto-Rotate", config.logging.autoRotate);
    printBoolSetting("Log to Serial", config.logging.logToSerial);
    
    printLine("");
    printSeparator();
    printLine("");
    
    printLine(ANSI_BOLD "Options:" ANSI_RESET);
    printLine("");
    printLine("  " ANSI_GREEN "[E]" ANSI_RESET " Enable/Disable Logging");
    printLine("  " ANSI_GREEN "[L]" ANSI_RESET " Set Log Level");
    printLine("  " ANSI_GREEN "[S]" ANSI_RESET " Set Max File Size");
    printLine("  " ANSI_GREEN "[R]" ANSI_RESET " Toggle Auto-Rotate");
    printLine("  " ANSI_GREEN "[T]" ANSI_RESET " Toggle Serial Output");
    printLine("");
    printLine("  " ANSI_CYAN "[M]" ANSI_RESET " Back to Settings Menu");
    printLine("");
    printLine("Enter option: ");
}

// ============================================================================
// System Settings Submenu
// ============================================================================

void MenuSystem::showSettingsSystem() {
    clearScreen();
    printHeader("System Settings");
    printBreadcrumb();
    
    ConfigManager& cfg = ConfigManager::getInstance();
    const DeviceConfig& config = cfg.getConfig();
    
    printLine(ANSI_BOLD "Current System Configuration:" ANSI_RESET);
    printLine("");
    
    printStringSetting("Device Name", config.system.deviceName);
    
    char line[128];
    snprintf(line, sizeof(line), "  Timezone Offset: " ANSI_CYAN "%+d" ANSI_RESET " minutes", config.system.timezoneOffset);
    printLine(line);
    
    printBoolSetting("NTP Enabled", config.system.ntpEnabled);
    printStringSetting("Escape Sequence", config.system.escapeSequence);
    printBoolSetting("Debug Mode", config.system.debugMode);
    
    printLine("");
    printSeparator();
    printLine("");
    
    printLine(ANSI_BOLD "Options:" ANSI_RESET);
    printLine("");
    printLine("  " ANSI_GREEN "[N]" ANSI_RESET " Change Device Name");
    printLine("  " ANSI_GREEN "[T]" ANSI_RESET " Set Timezone Offset");
    printLine("  " ANSI_GREEN "[P]" ANSI_RESET " Toggle NTP");
    printLine("  " ANSI_GREEN "[E]" ANSI_RESET " Change Escape Sequence");
    printLine("  " ANSI_GREEN "[D]" ANSI_RESET " Toggle Debug Mode");
    printLine("");
    printLine("  " ANSI_CYAN "[M]" ANSI_RESET " Back to Settings Menu");
    printLine("");
    printLine("Enter option: ");
}

// ============================================================================
// Statistics Menu
// ============================================================================

void MenuSystem::showStatsMenu() {
    clearScreen();
    printHeader("Statistics & Performance");
    printBreadcrumb();
    
    char line[128];
    
    printLine(ANSI_BOLD "Connection Statistics:" ANSI_RESET);
    snprintf(line, sizeof(line), "  Baud Rate:     " ANSI_CYAN "%lu" ANSI_RESET " bps", 
             (unsigned long)currentBaud);
    printLine(line);
    
    snprintf(line, sizeof(line), "  Bytes RX:      " ANSI_GREEN "%lu" ANSI_RESET, 
             (unsigned long)bytesRx);
    printLine(line);
    
    snprintf(line, sizeof(line), "  Bytes TX:      " ANSI_YELLOW "%lu" ANSI_RESET, 
             (unsigned long)bytesTx);
    printLine(line);
    
    uint32_t total = bytesRx + bytesTx;
    snprintf(line, sizeof(line), "  Total:         " ANSI_BOLD "%lu" ANSI_RESET " bytes", 
             (unsigned long)total);
    printLine(line);
    printLine("");
    
    printLine(ANSI_BOLD "System Info:" ANSI_RESET);
    printLine("  Device:        ESP32-C6");
    printLine("  Framework:     ESP-IDF 5.3.1");
    printLine("  Version:       SmvIT 0.2.0");
    printLine("");
    
    printSeparator();
    printLine("");
    printLine("  " ANSI_CYAN "[M]" ANSI_RESET " Back to Main Menu");
    printLine("");
}

// ============================================================================
// Hardware Info Menu
// ============================================================================

void MenuSystem::showHardwareInfo() {
    clearScreen();
    printHeader("Hardware Information");
    printBreadcrumb();
    
    char line[128];
    
    printLine(ANSI_BOLD "Detected Hardware Modules:" ANSI_RESET);
    printLine("");
    
    snprintf(line, sizeof(line), "  Display:       %s", 
             hasDisplay ? ANSI_GREEN "✓ Detected" ANSI_RESET : ANSI_RED "✗ Not Found" ANSI_RESET);
    printLine(line);
    
    snprintf(line, sizeof(line), "  SD Card:       %s", 
             hasSD ? ANSI_GREEN "✓ Detected" ANSI_RESET : ANSI_RED "✗ Not Found" ANSI_RESET);
    printLine(line);
    
    snprintf(line, sizeof(line), "  Accelerometer: %s", 
             hasAccel ? ANSI_GREEN "✓ Detected" ANSI_RESET : ANSI_RED "✗ Not Found" ANSI_RESET);
    printLine(line);
    
    printLine("");
    printSeparator();
    printLine("");
    printLine("  " ANSI_CYAN "[M]" ANSI_RESET " Back to Main Menu");
    printLine("");
}

// ============================================================================
// Help Menu
// ============================================================================

void MenuSystem::showHelpMenu() {
    clearScreen();
    printHeader("Help & Information");
    printBreadcrumb();
    
    printLine(ANSI_BOLD "Quick Start Guide:" ANSI_RESET);
    printLine("");
    printLine("1. Configure UART settings (baud rate, parity, etc.)");
    printLine("2. Enable Bluetooth if needed for wireless connection");
    printLine("3. Use Bridge Mode for transparent serial passthrough");
    printLine("4. Monitor Statistics for connection details");
    printLine("");
    
    printLine(ANSI_BOLD "Escape Sequence:" ANSI_RESET);
    printLine("  Type '~~~' to exit Bridge Mode");
    printLine("");
    
    printSeparator();
    printLine("");
    printLine("  " ANSI_CYAN "[M]" ANSI_RESET " Back to Main Menu");
    printLine("");
}

// ============================================================================
// Input Handling
// ============================================================================

void MenuSystem::handleInput(char c) {
    lastCommand = c;
    
    if (editingMode) {
        handleEditInput(c);
        return;
    }
    
    // Convert to uppercase for case-insensitive matching
    char cmd = (c >= 'a' && c <= 'z') ? (c - 32) : c;
    
    bool needRedraw = false;
    
    switch (currentScreen) {
        case MENU_MAIN:
            if (cmd == 'S') {
                navigateTo(MENU_SETTINGS);
                needRedraw = true;
            }
            else if (cmd == 'I') {
                navigateTo(MENU_STATS);
                needRedraw = true;
            }
            else if (cmd == 'H') {
                navigateTo(MENU_HARDWARE);
                needRedraw = true;
            }
            else if (cmd == '?') {
                navigateTo(MENU_HELP);
                needRedraw = true;
            }
            // B, D, R handled by main.cpp
            break;
            
        case MENU_SETTINGS:
            if (cmd == '1') {
                navigateTo(MENU_SETTINGS_UART);
                needRedraw = true;
            }
            else if (cmd == '2') {
                navigateTo(MENU_SETTINGS_BLUETOOTH);
                needRedraw = true;
            }
            else if (cmd == '3') {
                navigateTo(MENU_SETTINGS_DISPLAY);
                needRedraw = true;
            }
            else if (cmd == '4') {
                navigateTo(MENU_SETTINGS_LOGGING);
                needRedraw = true;
            }
            else if (cmd == '5') {
                navigateTo(MENU_SETTINGS_SYSTEM);
                needRedraw = true;
            }
            else if (cmd == '0') {
                ConfigManager::getInstance().reset();
                printLine(ANSI_GREEN "\n✓ Settings reset to factory defaults!\n" ANSI_RESET);
                vTaskDelay(pdMS_TO_TICKS(2000));
                needRedraw = true;
            }
            else if (cmd == 'M') {
                navigateBack();
                needRedraw = true;
            }
            break;
            
        case MENU_SETTINGS_BLUETOOTH:
            if (cmd == 'E') {
                handleBluetoothToggle();
                needRedraw = true;
            }
            else if (cmd == 'N') handleBluetoothNameEdit();
            else if (cmd == 'A') {
                handleBluetoothAutoAdvertiseToggle();
                needRedraw = true;
            }
            else if (cmd == 'P') {
                handleBluetoothPairingToggle();
                needRedraw = true;
            }
            else if (cmd == 'C') handleBluetoothPinEdit();
            else if (cmd == 'I') handleBluetoothTxPowerEdit();
            else if (cmd == 'M') {
                navigateBack();
                needRedraw = true;
            }
            break;
            
        case MENU_SETTINGS_UART:
            if (cmd == 'B') handleUARTBaudEdit();
            else if (cmd == 'A') {
                handleUARTAutoDetectToggle();
                needRedraw = true;
            }
            else if (cmd == 'P') handleUARTParityEdit();
            else if (cmd == 'S') handleUARTStopBitsEdit();
            else if (cmd == 'F') handleUARTFlowControlEdit();
            else if (cmd == 'M') {
                navigateBack();
                needRedraw = true;
            }
            break;
            
        case MENU_SETTINGS_DISPLAY:
        case MENU_SETTINGS_LOGGING:
        case MENU_SETTINGS_SYSTEM:
            if (cmd == 'M') {
                navigateBack();
                needRedraw = true;
            }
            break;
            
        case MENU_STATS:
        case MENU_HARDWARE:
        case MENU_HELP:
            if (cmd == 'M') {
                navigateBack();
                needRedraw = true;
            }
            break;
    }
    
    // Redraw the menu if navigation occurred
    if (needRedraw) {
        show();
    }
}

void MenuSystem::show() {
    switch (currentScreen) {
        case MENU_MAIN: showMainMenu(); break;
        case MENU_SETTINGS: showSettingsMenu(); break;
        case MENU_SETTINGS_UART: showSettingsUART(); break;
        case MENU_SETTINGS_BLUETOOTH: showSettingsBluetooth(); break;
        case MENU_SETTINGS_DISPLAY: showSettingsDisplay(); break;
        case MENU_SETTINGS_LOGGING: showSettingsLogging(); break;
        case MENU_SETTINGS_SYSTEM: showSettingsSystem(); break;
        case MENU_STATS: showStatsMenu(); break;
        case MENU_HARDWARE: showHardwareInfo(); break;
        case MENU_HELP: showHelpMenu(); break;
    }
}

void MenuSystem::update() {
    // Placeholder for future animation/refresh logic
}

// ============================================================================
// Bluetooth Settings Handlers
// ============================================================================

void MenuSystem::handleBluetoothToggle() {
    ConfigManager& cfg = ConfigManager::getInstance();
    bool current = cfg.getConfig().bluetooth.enabled;
    cfg.setBluetoothEnabled(!current);
    printLine(current ? ANSI_YELLOW "\n✓ Bluetooth disabled\n" ANSI_RESET : 
                       ANSI_GREEN "\n✓ Bluetooth enabled\n" ANSI_RESET);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void MenuSystem::handleBluetoothNameEdit() {
    printLine(ANSI_YELLOW "\nEnter new Bluetooth device name (max 31 chars):" ANSI_RESET);
    printLine("(Press Enter when done, ESC to cancel)");
    printf("> ");
    startEdit("Bluetooth Name");
}

void MenuSystem::handleBluetoothAutoAdvertiseToggle() {
    ConfigManager& cfg = ConfigManager::getInstance();
    bool current = cfg.getConfig().bluetooth.autoAdvertise;
    cfg.setBluetoothAutoAdvertise(!current);
    printLine(current ? ANSI_YELLOW "\n✓ Auto-advertise disabled\n" ANSI_RESET :
                       ANSI_GREEN "\n✓ Auto-advertise enabled\n" ANSI_RESET);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void MenuSystem::handleBluetoothPairingToggle() {
    ConfigManager& cfg = ConfigManager::getInstance();
    bool current = cfg.getConfig().bluetooth.requirePairing;
    cfg.setBluetoothRequirePairing(!current);
    printLine(current ? ANSI_YELLOW "\n✓ Pairing not required\n" ANSI_RESET :
                       ANSI_GREEN "\n✓ Pairing required\n" ANSI_RESET);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void MenuSystem::handleBluetoothPinEdit() {
    printLine(ANSI_YELLOW "\nEnter 6-digit PIN code:" ANSI_RESET);
    printLine("(Press Enter when done, ESC to cancel)");
    printf("> ");
    startEdit("PIN Code");
}

void MenuSystem::handleBluetoothTxPowerEdit() {
    printLine(ANSI_YELLOW "\nEnter TX power in dBm (-12 to +9):" ANSI_RESET);
    printLine("(Press Enter when done, ESC to cancel)");
    printf("> ");
    startEdit("TX Power");
}

// ============================================================================
// UART Settings Handlers
// ============================================================================

void MenuSystem::handleUARTBaudEdit() {
    printLine(ANSI_YELLOW "\nEnter baud rate (300-921600):" ANSI_RESET);
    printLine("(Press Enter when done, ESC to cancel)");
    printf("> ");
    startEdit("Baud Rate");
}

void MenuSystem::handleUARTAutoDetectToggle() {
    ConfigManager& cfg = ConfigManager::getInstance();
    bool current = cfg.getConfig().uart.autoDetect;
    cfg.setUARTAutoDetect(!current);
    printLine(current ? ANSI_YELLOW "\n✓ Auto-detect disabled\n" ANSI_RESET :
                       ANSI_GREEN "\n✓ Auto-detect enabled\n" ANSI_RESET);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void MenuSystem::handleUARTParityEdit() {
    printLine(ANSI_YELLOW "\nSelect parity:" ANSI_RESET);
    printLine("  0 = None");
    printLine("  1 = Even");
    printLine("  2 = Odd");
    printf("> ");
    startEdit("Parity");
}

void MenuSystem::handleUARTStopBitsEdit() {
    printLine(ANSI_YELLOW "\nEnter stop bits (1 or 2):" ANSI_RESET);
    printf("> ");
    startEdit("Stop Bits");
}

void MenuSystem::handleUARTFlowControlEdit() {
    printLine(ANSI_YELLOW "\nSelect flow control:" ANSI_RESET);
    printLine("  0 = None");
    printLine("  1 = RTS/CTS");
    printLine("  2 = XON/XOFF");
    printf("> ");
    startEdit("Flow Control");
}

// ============================================================================
// Input Editing Functions
// ============================================================================

void MenuSystem::startEdit(const char* prompt) {
    editingMode = true;
    inputPosition = 0;
    memset(inputBuffer, 0, sizeof(inputBuffer));
}

void MenuSystem::handleEditInput(char c) {
    if (c == '\r' || c == '\n') {
        finishEdit();
    } else if (c == 27) { // ESC
        cancelEdit();
    } else if (c == 127 || c == 8) { // Backspace
        if (inputPosition > 0) {
            inputPosition--;
            inputBuffer[inputPosition] = '\0';
            printf("\b \b");
        }
    } else if (inputPosition < sizeof(inputBuffer) - 1) {
        inputBuffer[inputPosition++] = c;
        printf("%c", c);
    }
}

bool MenuSystem::finishEdit() {
    editingMode = false;
    ConfigManager& cfg = ConfigManager::getInstance();
    
    // Process based on current screen and setting
    // This is a simplified version - full implementation would track which setting is being edited
    printf("\n");
    
    if (currentScreen == MENU_SETTINGS_BLUETOOTH) {
        // Example: set bluetooth name
        if (strlen(inputBuffer) > 0) {
            cfg.setBluetoothDeviceName(inputBuffer);
            printLine(ANSI_GREEN "✓ Bluetooth name updated!" ANSI_RESET);
        }
    }
    
    vTaskDelay(pdMS_TO_TICKS(1500));
    return true;
}

void MenuSystem::cancelEdit() {
    editingMode = false;
    printLine(ANSI_RED "\n✗ Cancelled" ANSI_RESET);
    vTaskDelay(pdMS_TO_TICKS(1000));
}
