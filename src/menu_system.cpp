#include "menu_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include <cstring>
#include <cstdio>

static const char *TAG = "MENU";

// ANSI escape codes for terminal formatting
#define ANSI_CLEAR "\033[2J\033[H"
#define ANSI_BOLD "\033[1m"
#define ANSI_RESET "\033[0m"
#define ANSI_GREEN "\033[32m"
#define ANSI_CYAN "\033[36m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_RED "\033[31m"

MenuSystem::MenuSystem() :
    currentScreen(MENU_MAIN),
    lastCommand(0),
    currentBaud(0),
    bytesRx(0),
    bytesTx(0),
    hasDisplay(false),
    hasSD(false),
    hasAccel(false)
{
}

void MenuSystem::printLine(const char* line) {
    uart_write_bytes(UART_NUM_0, line, strlen(line));
    uart_write_bytes(UART_NUM_0, "\r\n", 2);
}

void MenuSystem::printHeader(const char* title) {
    char header[200];  // Increased buffer size for ANSI codes
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
    uart_write_bytes(UART_NUM_0, ANSI_CLEAR, strlen(ANSI_CLEAR));
}

void MenuSystem::showMainMenu() {
    clearScreen();
    printHeader("SmvIT USB-TTL Bridge - Main Menu");
    printLine("");
    
    char line[80];
    
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

void MenuSystem::showSettingsMenu() {
    clearScreen();
    printHeader("Settings");
    printLine("");
    
    char line[80];
    
    printLine(ANSI_BOLD "Baud Rate Settings:" ANSI_RESET);
    printLine("  [1] 9600 bps");
    printLine("  [2] 19200 bps");
    printLine("  [3] 38400 bps");
    printLine("  [4] 57600 bps");
    printLine("  [5] 115200 bps");
    printLine("  [A] Auto-detect (intelligent)");
    printLine("");
    
    printLine(ANSI_BOLD "Other Settings:" ANSI_RESET);
    printLine("  [L] Toggle logging");
    printLine("  [E] Escape sequence: ~~~");
    printLine("");
    
    printSeparator();
    printLine("");
    printLine("  [M] Back to Main Menu");
    printLine("");
    printLine("Enter option: ");
}

void MenuSystem::showStatsMenu() {
    clearScreen();
    printHeader("Statistics & Performance");
    printLine("");
    
    char line[80];
    
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
    printLine("  Version:       SmvIT 0.1.0");
    printLine("");
    
    printLine(ANSI_BOLD "Features:" ANSI_RESET);
    printLine("  ✓ Intelligent baud detection");
    printLine("  ✓ Bridge mode with escape sequence");
    printLine("  ✓ Hardware auto-detection");
    printLine("  ✓ BLE serial bridge (stub)");
    printLine("");
    
    printSeparator();
    printLine("");
    printLine("  [M] Back to Main Menu");
    printLine("");
    printLine("Press any key to continue...");
}

void MenuSystem::showHardwareInfo() {
    clearScreen();
    printHeader("Hardware Information");
    printLine("");
    
    char line[80];
    
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
    printLine(ANSI_BOLD "Pin Configuration:" ANSI_RESET);
    printLine("");
    
    snprintf(line, sizeof(line), "  Target RX:     GPIO %d", RX_PIN);
    printLine(line);
    snprintf(line, sizeof(line), "  Target TX:     GPIO %d", TX_PIN);
    printLine(line);
    snprintf(line, sizeof(line), "  I2C SDA:       GPIO %d", I2C_SDA_PIN);
    printLine(line);
    snprintf(line, sizeof(line), "  I2C SCL:       GPIO %d", I2C_SCL_PIN);
    printLine(line);
    snprintf(line, sizeof(line), "  SD CS:         GPIO %d", SD_CS);
    printLine(line);
    printLine("");
    
    printSeparator();
    printLine("");
    printLine("  [M] Back to Main Menu");
    printLine("");
    printLine("Press any key to continue...");
}

void MenuSystem::showHelpMenu() {
    clearScreen();
    printHeader("Help & Quick Start Guide");
    printLine("");
    
    printLine(ANSI_BOLD "Quick Start:" ANSI_RESET);
    printLine("");
    printLine("  1. Connect target device to RX/TX pins");
    printLine("  2. Press [D] to auto-detect baud rate");
    printLine("  3. Wait for detection (analyzes bit timing)");
    printLine("  4. Automatically enters bridge mode");
    printLine("  5. Type '~~~' to return to menu");
    printLine("");
    
    printLine(ANSI_BOLD "Bridge Mode:" ANSI_RESET);
    printLine("");
    printLine("  Bridge mode provides transparent serial");
    printLine("  passthrough between USB and target device.");
    printLine("  All data flows bidirectionally in real-time.");
    printLine("");
    printLine("  Escape: Type '" ANSI_YELLOW "~~~" ANSI_RESET "' to exit bridge mode");
    printLine("");
    
    printLine(ANSI_BOLD "Features:" ANSI_RESET);
    printLine("");
    printLine("  • Intelligent baud detection via bit timing");
    printLine("  • Manual baud rate selection");
    printLine("  • Hardware auto-detection (I2C scan)");
    printLine("  • Real-time statistics");
    printLine("  • SD card logging (when detected)");
    printLine("  • BLE serial bridge (future)");
    printLine("");
    
    printSeparator();
    printLine("");
    printLine("  [M] Back to Main Menu");
    printLine("");
    printLine("Press any key to continue...");
}

void MenuSystem::show() {
    switch (currentScreen) {
        case MENU_MAIN:
            showMainMenu();
            break;
        case MENU_SETTINGS:
            showSettingsMenu();
            break;
        case MENU_STATS:
            showStatsMenu();
            break;
        case MENU_HARDWARE:
            showHardwareInfo();
            break;
        case MENU_HELP:
            showHelpMenu();
            break;
    }
}

void MenuSystem::update() {
    // Menu updates handled by show()
}

void MenuSystem::handleInput(char c) {
    // Convert to uppercase for easier comparison
    char cmd = (c >= 'a' && c <= 'z') ? (c - 32) : c;
    
    lastCommand = cmd;
    
    ESP_LOGI(TAG, "Menu input: %c (screen: %d)", cmd, currentScreen);
    
    // Common commands across all screens
    if (cmd == 'M' && currentScreen != MENU_MAIN) {
        currentScreen = MENU_MAIN;
        show();
        return;
    }
    
    // Screen-specific commands
    switch (currentScreen) {
        case MENU_MAIN:
            switch (cmd) {
                case 'B':  // Bridge mode
                case 'D':  // Detect
                case 'R':  // Reset
                    // These are handled by main application
                    break;
                    
                case 'S':  // Settings
                    currentScreen = MENU_SETTINGS;
                    show();
                    break;
                    
                case 'I':  // Statistics
                    currentScreen = MENU_STATS;
                    show();
                    break;
                    
                case 'H':  // Hardware
                    currentScreen = MENU_HARDWARE;
                    show();
                    break;
                    
                case '?':  // Help
                    currentScreen = MENU_HELP;
                    show();
                    break;
            }
            break;
            
        case MENU_SETTINGS:
            switch (cmd) {
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case 'A':
                case 'L':
                case 'E':
                    // Handled by main application
                    break;
            }
            break;
            
        case MENU_STATS:
        case MENU_HARDWARE:
        case MENU_HELP:
            // Any key returns to main menu
            currentScreen = MENU_MAIN;
            show();
            break;
    }
}
