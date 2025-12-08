# 🔧 SmvIT API Documentation

Complete developer API reference for the SmvIT USB-TTL Bridge firmware.

**Target Audience:** Firmware developers and advanced users  
**Last Updated:** December 8, 2025

---

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Core Modules](#core-modules)
- [Public APIs](#public-apis)
- [Configuration](#configuration)
- [Development Guidelines](#development-guidelines)
- [Common Tasks](#common-tasks)
- [Debugging](#debugging)

---

## Architecture Overview

### System Architecture

```
┌───────────────────────────────────────────┐
│         Main Application (main.cpp)       │
│  - System initialization                  │
│  - State machine management               │
│  - Hardware coordination                  │
└───────────────────────────────────────────┘
           │                    │
    ┌──────┴──────┐      ┌──────┴──────┐
    │   Bridge    │      │   Hardware  │
    │   Mode      │      │   Detector  │
    ├─────────────┤      ├─────────────┤
    │- UART RX/TX │      │- I2C Scan   │
    │- Statistics │      │- GPIO Check │
    │- Logging    │      │- Status     │
    └──────┬──────┘      └──────┬──────┘
           │                    │
    ┌──────┴──────┐      ┌──────┴──────┐
    │    Baud     │      │   Display   │
    │  Detector   │      │   Manager   │
    ├─────────────┤      ├─────────────┤
    │- GPIO IRQ   │      │- TFT Driver │
    │- Timing     │      │- Screens    │
    │- Detection  │      │- DMA        │
    └─────────────┘      └──────┬──────┘
           │                    │
           └────────┬───────────┘
                    │
            ┌───────┴────────┐
            │    SD Logger   │
            ├────────────────┤
            │- FAT FS        │
            │- File Ops      │
            │- Timestamping  │
            └────────────────┘
```

### State Machine

```
BOOTING
   ↓
WAITING (for target device)
   ↓
ANALYZING (detecting baud)
   ├→ ANALYZING_FAILED ↔ WAITING
   │
   ↓
FOUND_SPEED (baud detected)
   ↓
BRIDGE_MODE (transparent passthrough)
   ├→ MENU_STATE (user menu)
   │
   ↓
TESTING (comm test)
   ↓
RUNNING (normal operation)
```

---

## Core Modules

### 1. Main Application (`src/main.cpp`)

**Purpose:** System initialization, state management, and module coordination.

**Key Functions:**

#### `void setup_hardware()`
Initializes all hardware components during boot.

```cpp
void setup_hardware();
```

**Initializes:**
- UART console for USB serial
- Hardware detector
- Display manager (if enabled)
- SD card logger
- Baud detector
- Bluetooth manager
- Bridge mode
- Menu system

**Called:** Once during boot in `app_main()`

#### `void changeState(SystemState newState)`
Transitions system to a new state.

```cpp
void changeState(SystemState newState);
```

**Parameters:**
- `newState` - Target system state

**Side Effects:**
- Updates display (if present)
- Logs state change to SD card
- Triggers state-specific handlers

**States:**
```cpp
enum SystemState {
    STATE_BOOTING,
    STATE_WAITING,
    STATE_ANALYZING,
    STATE_ANALYZING_FAILED,
    STATE_FOUND_SPEED,
    STATE_MENU,
    STATE_BRIDGE_MODE,
    STATE_TESTING,
    STATE_RUNNING
};
```

#### `static uint32_t millis()`
Returns milliseconds since boot using ESP timer.

```cpp
static uint32_t millis();
```

**Returns:** Milliseconds since system start (uint32_t)

**Usage:** Timing, delays, and timeout calculations

#### `void handleBridgeModeState()`
Manages transparent serial passthrough operation.

```cpp
void handleBridgeModeState();
```

**Performs:**
- Reads from target UART
- Sends to host USB UART
- Updates statistics
- Logs to SD card
- Checks for escape sequence `~~~`

**Called:** Repeatedly while in `STATE_BRIDGE_MODE`

#### `void handleAnalyzingState()`
Runs baud rate detection algorithm.

```cpp
void handleAnalyzingState();
```

**Performs:**
- Calls baud detector
- Displays progress
- Handles timeout
- Transitions on success/failure

**Called:** While in `STATE_ANALYZING`

---

### 2. Hardware Detector (`include/hardware_detector.h` / `src/hardware_detector.cpp`)

**Purpose:** Scan and detect connected optional hardware.

**Public API:**

```cpp
class HardwareDetector {
public:
    HardwareConfig scanAll();
    bool probeDisplay();
    bool probeSDCard();
    bool probeGPS();
    HardwareConfig getStatus();
};
```

#### `HardwareConfig scanAll()`
Scans for all connected hardware.

```cpp
HardwareConfig hwConfig = hwDetector.scanAll();
```

**Returns:** `HardwareConfig` structure with detection results

**Scans:**
- Display (via I2C address scanning)
- SD Card (via SPI initialization)
- GPS/GNSS (stub)

**Data Structure:**
```cpp
struct HardwareConfig {
    bool displayPresent;
    bool sdCardPresent;
    bool gpsPresent;
    uint8_t i2cAddresses[16];
    uint8_t foundDevices;
};
```

#### `bool probeDisplay()`
Test for display hardware.

```cpp
bool hasDisplay = hwDetector.probeDisplay();
```

**Returns:** `true` if display detected, `false` otherwise

#### `bool probeSDCard()`
Test for SD card module.

```cpp
bool hasSD = hwDetector.probeSDCard();
```

**Returns:** `true` if SD card found, `false` otherwise

---

### 3. Baud Detector (`include/baud_detector.h` / `src/baud_detector.cpp`)

**Purpose:** Automatically detect target device baud rate through GPIO timing analysis.

**Public API:**

```cpp
class BaudDetector {
public:
    void begin();
    void start();
    bool isComplete();
    uint32_t getDetectedBaud();
    bool isSuccess();
    float getConfidence();
    void reset();
};
```

#### `void begin()`
Initialize baud detector hardware (GPIO interrupts, timers).

```cpp
baudDetector.begin();
```

**Initializes:**
- GPIO ISR for RX pin (GPIO16)
- Timing arrays
- Detection state

**Called Once:** During `setup_hardware()`

#### `void start()`
Begin baud rate analysis.

```cpp
baudDetector.start();
```

**Starts:** GPIO interrupt monitoring and timing analysis

**Duration:** ~2-5 seconds typically

**Call After:** Connecting target device

#### `bool isComplete()`
Check if detection is complete.

```cpp
if (baudDetector.isComplete()) {
    uint32_t baud = baudDetector.getDetectedBaud();
}
```

**Returns:** `true` when analysis finished, `false` if still running

#### `uint32_t getDetectedBaud()`
Get the detected baud rate.

```cpp
uint32_t baud = baudDetector.getDetectedBaud();
// Possible values: 9600, 19200, 38400, 57600, 115200
```

**Returns:** Baud rate in bits per second

**Valid Only After:** `isComplete()` returns `true`

#### `bool isSuccess()`
Check if detection succeeded.

```cpp
if (baudDetector.isSuccess()) {
    printf("Baud: %d bps\n", baudDetector.getDetectedBaud());
} else {
    printf("Detection failed\n");
}
```

**Returns:** `true` if baud successfully detected, `false` if detection failed

#### `float getConfidence()`
Get confidence level of detection (0.0 - 1.0).

```cpp
float confidence = baudDetector.getConfidence();
if (confidence > 0.8f) {
    printf("High confidence\n");
}
```

**Returns:** Confidence as float (0.0 = no confidence, 1.0 = absolute)

#### `void reset()`
Reset detector for new analysis session.

```cpp
baudDetector.reset();
baudDetector.start();
```

**Clears:** Previous detection results, restarts monitoring

---

### 4. Display Manager (`include/display_manager.h` / `src/display_manager.cpp`)

**Purpose:** Manage TFT display and screen rendering.

**Public API:**

```cpp
class DisplayManager {
public:
    bool begin();
    void setState(SystemState state);
    void setStatus(const char* status);
    void setProgress(int percent);
    void updateBaudInfo(uint32_t baud, bool detected);
    void updateBridgeStats(uint32_t rxBytes, uint32_t txBytes);
    void update();
    void clearDisplay();
    bool isPresent();
};
```

#### `bool begin()`
Initialize display hardware.

```cpp
if (display.begin()) {
    printf("Display initialized\n");
} else {
    printf("Display not found\n");
}
```

**Returns:** `true` if display found and initialized, `false` if absent

**Initializes:**
- SPI interface
- GPIO pins (DC, RST, BL)
- Display memory
- Default font

#### `void setState(SystemState state)`
Change displayed screen based on system state.

```cpp
display.setState(STATE_ANALYZING);
```

**State-Screen Mapping:**
- `STATE_BOOTING` → Boot splash screen
- `STATE_MENU` → Menu screen
- `STATE_ANALYZING` → Baud detection progress
- `STATE_BRIDGE_MODE` → Live statistics screen
- `STATE_TESTING` → Test results screen

#### `void setStatus(const char* status)`
Display status message.

```cpp
display.setStatus("Initializing SD card...");
```

**Displays:** Text message on screen (max ~40 chars)

#### `void setProgress(int percent)`
Show progress bar (0-100%).

```cpp
for (int i = 0; i <= 100; i += 10) {
    display.setProgress(i);
    vTaskDelay(pdMS_TO_TICKS(100));
}
```

**Parameter:** Progress percentage (0-100)

#### `void updateBaudInfo(uint32_t baud, bool detected)`
Update baud rate display.

```cpp
display.updateBaudInfo(115200, true);
```

**Parameters:**
- `baud` - Baud rate in bps
- `detected` - `true` if auto-detected, `false` if manual

#### `void updateBridgeStats(uint32_t rxBytes, uint32_t txBytes)`
Update bridge mode statistics display.

```cpp
display.updateBridgeStats(1024, 512);
```

**Parameters:**
- `rxBytes` - Total received bytes
- `txBytes` - Total transmitted bytes

**Updates** screen in real-time

#### `void update()`
Refresh display with current data.

```cpp
// Called periodically (20 Hz = 50 ms intervals)
display.update();
```

**Purpose:** Renders all pending changes to screen

**Call Frequency:** Every 50 ms typically

#### `void clearDisplay()`
Erase entire screen.

```cpp
display.clearDisplay();
```

**Fills:** Screen with black

#### `bool isPresent()`
Check if display hardware is available.

```cpp
if (display.isPresent()) {
    display.update();
}
```

**Returns:** `true` if display initialized, `false` otherwise

---

### 5. SD Logger (`include/sd_logger.h` / `src/sd_logger.cpp`)

**Purpose:** Log system events and data to SD card with timestamps.

**Public API:**

```cpp
class SDLogger {
public:
    bool begin();
    void log(LogLevel level, const char* format, ...);
    void logData(const char* label, const uint8_t* data, size_t len);
    void flush();
    bool isReady();
    const char* getLogPath();
    void end();
};

enum LogLevel {
    LOG_INFO,
    LOG_DATA,
    LOG_BAUD,
    LOG_STATE,
    LOG_TEST,
    LOG_ERROR
};
```

#### `bool begin()`
Initialize SD card and create log file.

```cpp
if (logger.begin()) {
    printf("SD card ready, logging to: %s\n", logger.getLogPath());
} else {
    printf("SD card initialization failed\n");
}
```

**Returns:** `true` if successful, `false` if SD card not available

**Creates:**
- Mount point at `/sdcard/`
- Log directory `/sdcard/logs/`
- Log file `/sdcard/logs/log_YYYYMMDD_HHMMSS.txt`

#### `void log(LogLevel level, const char* format, ...)`
Write formatted log entry.

```cpp
logger.log(LOG_INFO, "Baud detected: %d bps", 115200);
logger.log(LOG_STATE, "State: WAITING -> ANALYZING");
logger.log(LOG_ERROR, "SD card not found");
```

**Format:** Uses `printf`-style formatting

**Auto-Timestamp:** `[YYYY-MM-DD HH:MM:SS]` prepended

**Auto-Flush:** Every 10 entries written

**Log Levels:**
- `LOG_INFO` - System information
- `LOG_DATA` - Transmitted/received data
- `LOG_BAUD` - Baud detection events
- `LOG_STATE` - State machine transitions
- `LOG_TEST` - Communication test results
- `LOG_ERROR` - Error conditions

#### `void logData(const char* label, const uint8_t* data, size_t len)`
Log binary data as hex dump.

```cpp
uint8_t buffer[11] = "Hello World";
logger.logData("TX", buffer, 11);
// Logs: [2025-12-08 14:31:06] [DATA] TX: 48 65 6C 6C 6F ...
```

**Parameters:**
- `label` - Data type label (e.g., "TX", "RX")
- `data` - Pointer to byte buffer
- `len` - Number of bytes to log

**Max Size:** 64 bytes per log entry (longer data truncated with "...")

#### `void flush()`
Force write all buffered data to SD card.

```cpp
logger.flush();
```

**Purpose:** Ensure data is not lost in power failure

**Call:** Before critical state changes

#### `bool isReady()`
Check if logging is available.

```cpp
if (logger.isReady()) {
    logger.log(LOG_INFO, "SD card is ready");
}
```

**Returns:** `true` if SD card initialized and mounted

#### `const char* getLogPath()`
Get full path to current log file.

```cpp
printf("Log file: %s\n", logger.getLogPath());
// Output: Log file: /sdcard/logs/log_20251208_143052.txt
```

**Returns:** Full file path string

#### `void end()`
Close log file and unmount SD card.

```cpp
logger.flush();
logger.end();
```

**Call:** Before shutting down or restarting

---

### 6. Bridge Mode (`include/bridge_mode.h` / `src/bridge_mode.cpp`)

**Purpose:** Transparent serial passthrough between host and target device.

**Public API:**

```cpp
class BridgeMode {
public:
    void begin(uint32_t baudRate);
    void run();
    bool detectEscapeSequence(uint8_t byte);
    void getStatistics(BridgeStats* stats);
    void reset();
    bool isActive();
};

struct BridgeStats {
    uint32_t bytesRX;
    uint32_t bytesTX;
    uint32_t startTime;
    float throughputRX;
    float throughputTX;
};
```

#### `void begin(uint32_t baudRate)`
Initialize bridge with target baud rate.

```cpp
bridge.begin(115200);
```

**Configures:**
- UART1 to target device
- Baud rate
- TX/RX buffers
- Statistics counters

#### `void run()`
Execute bridge operation (should be called in loop).

```cpp
while (inBridgeMode) {
    bridge.run();
    vTaskDelay(pdMS_TO_TICKS(10));
}
```

**Operations:**
- Reads from target UART
- Sends to host USB UART
- Reads from host
- Sends to target
- Updates statistics
- Logs data

**Non-blocking:** Returns immediately

#### `bool detectEscapeSequence(uint8_t byte)`
Check for bridge exit sequence (`~~~`).

```cpp
uint8_t rxByte;
if (uart_read_bytes(UART_NUM_0, &rxByte, 1, 0) > 0) {
    if (bridge.detectEscapeSequence(rxByte)) {
        printf("Exit sequence detected\n");
        // Exit bridge mode
    }
}
```

**Sequence:** Type `~~~` (three tildes) to exit

**Returns:** `true` when sequence complete, `false` otherwise

#### `void getStatistics(BridgeStats* stats)`
Retrieve current operation statistics.

```cpp
BridgeStats stats;
bridge.getStatistics(&stats);
printf("RX: %d bytes, TX: %d bytes\n", stats.bytesRX, stats.bytesTX);
printf("RX Rate: %.2f KB/s\n", stats.throughputRX / 1024.0f);
```

**Retrieves:**
- Total bytes transmitted/received
- Session start time
- Current throughput rates

#### `void reset()`
Clear statistics and counters.

```cpp
bridge.reset();
```

**Clears:** All counters, starts fresh statistics

#### `bool isActive()`
Check if bridge mode is running.

```cpp
if (bridge.isActive()) {
    printf("Bridge is active\n");
}
```

**Returns:** `true` if actively bridging, `false` otherwise

---

### 7. Menu System (`include/menu_system.h` / `src/menu_system.cpp`)

**Purpose:** Interactive command-line menu interface.

**Public API:**

```cpp
class MenuSystem {
public:
    void begin();
    void handleInput(char cmd);
    void displayMenu();
    void displayHelp();
    void displayInfo();
    char waitForInput(uint32_t timeoutMs);
    bool isActive();
};
```

#### `void begin()`
Initialize menu system.

```cpp
menu.begin();
```

**Initializes:** Terminal, displays initial menu

#### `void handleInput(char cmd)`
Process menu command.

```cpp
char command = 'D';  // Detect baud
menu.handleInput(command);
```

**Valid Commands:**
- `D` - Detect baud rate
- `B` - Enter bridge mode
- `S` - Settings menu
- `I` - Device information
- `H` - Help
- `T` - Run tests
- `X` - Exit menu

#### `void displayMenu()`
Draw menu on screen/terminal.

```cpp
menu.displayMenu();
```

**Shows:** Available commands and status

#### `void displayHelp()`
Display command descriptions.

```cpp
menu.displayHelp();
```

**Shows:** Help text for all commands

#### `void displayInfo()`
Show device information.

```cpp
menu.displayInfo();
```

**Displays:**
- Hardware status
- Statistics
- Uptime
- Memory usage

#### `char waitForInput(uint32_t timeoutMs)`
Wait for user input with timeout.

```cpp
char cmd = menu.waitForInput(1000);  // Wait 1 second
if (cmd != 0) {
    printf("User pressed: %c\n", cmd);
}
```

**Returns:** Character entered, or 0 if timeout

**Blocks:** Up to specified milliseconds

#### `bool isActive()`
Check if menu is currently active.

```cpp
if (!menu.isActive()) {
    // In bridge mode, not menu
}
```

**Returns:** `true` if showing menu, `false` otherwise

---

## Public APIs

### Initialization Flow

```cpp
// In app_main() or similar:
setup_hardware();          // Initialize everything
changeState(STATE_MENU);   // Enter menu
```

### Main Loop Pattern

```cpp
void app_main(void) {
    setup_hardware();
    
    while(1) {
        switch(currentState) {
            case STATE_MENU:
                handleMenuState();
                break;
            case STATE_BRIDGE_MODE:
                handleBridgeModeState();
                break;
            case STATE_ANALYZING:
                handleAnalyzingState();
                break;
            // ... other states
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

### Typical Bridge Session

```cpp
// User presses B in menu
// System calls:
changeState(STATE_ANALYZING);
baudDetector.start();

// Wait for detection
while (!baudDetector.isComplete()) {
    vTaskDelay(pdMS_TO_TICKS(100));
}

if (baudDetector.isSuccess()) {
    uint32_t detectedBaud = baudDetector.getDetectedBaud();
    bridge.begin(detectedBaud);
    changeState(STATE_BRIDGE_MODE);
    
    while (inBridgeMode) {
        bridge.run();
        if (bridge.detectEscapeSequence(...)) {
            inBridgeMode = false;
            changeState(STATE_MENU);
        }
    }
}
```

---

## Configuration

### Build Configuration

**File:** `platformio.ini`

```ini
[env:esp32c6]
platform = espressif32@6.9.0
board = esp32-c6-devkitc-1
framework = espidf
build_flags = -DCONFIG_LOG_DEFAULT_LEVEL_DEBUG=1
```

### Feature Flags

**File:** `src/main.cpp`, line ~60

```cpp
bool tryDisplay = false;   // Enable/disable display
bool trySDCard = true;     // Enable/disable SD card
bool tryBLE = true;        // Enable/disable Bluetooth
```

### Hardware Pins

**File:** `include/config.h`

```cpp
// UART Bridge pins
#define UART_RX_PIN    16
#define UART_TX_PIN    17
#define UART_NUM       UART_NUM_1

// Display pins (SPI2)
#define TFT_MOSI_PIN   7
#define TFT_MISO_PIN   2
#define TFT_SCLK_PIN   6
#define TFT_CS_PIN     10
#define TFT_DC_PIN     3
#define TFT_RST_PIN    4
#define TFT_BL_PIN     5

// SD Card pins (SPI2)
#define SD_CS_PIN      1
```

### Memory Configuration

**File:** `sdkconfig.esp32c6`

Default settings optimized for available RAM. Adjust if adding features.

---

## Development Guidelines

### Code Style

- **Naming:** CamelCase for classes, snake_case for functions/variables
- **Comments:** Document public methods and non-obvious logic
- **Formatting:** Use 4-space indentation
- **Includes:** Include guards in headers, organize includes

### Adding New Module

**Steps:**

1. **Create header** (`include/my_module.h`)
```cpp
#pragma once

class MyModule {
public:
    bool begin();
    void doSomething();
    bool isReady();
};
```

2. **Create implementation** (`src/my_module.cpp`)
```cpp
#include "my_module.h"
#include "esp_log.h"

static const char *TAG = "MY_MODULE";

bool MyModule::begin() {
    ESP_LOGI(TAG, "Initializing...");
    return true;
}
```

3. **Add to main.cpp**
```cpp
#include "my_module.h"

MyModule myModule;

void setup_hardware() {
    // ...
    myModule.begin();
}
```

### Error Handling

Use ESP-IDF error codes:

```cpp
#include "esp_err.h"

esp_err_t result = some_operation();
if (result != ESP_OK) {
    ESP_LOGE(TAG, "Operation failed: %s", esp_err_to_name(result));
    return false;
}
```

### Logging

Use ESP-IDF logging macros:

```cpp
#include "esp_log.h"

ESP_LOGI(TAG, "Info message");        // INFO level
ESP_LOGW(TAG, "Warning");             // WARNING level
ESP_LOGE(TAG, "Error: %d", errno);    // ERROR level
ESP_LOGD(TAG, "Debug info");          // DEBUG level
```

### Memory Management

ESP32-C6 has limited RAM (~200 KB free):

- Allocate large buffers statically or on stack
- Use `malloc()` sparingly, always `free()`
- Monitor free heap: `printf("Free heap: %d\n", esp_get_free_heap_size());`
- Use `IRAM_ATTR` for performance-critical code

### FreeRTOS Integration

Current implementation is single-threaded. For multi-tasking:

```cpp
void myTask(void *param) {
    while(1) {
        // Do work
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setup_hardware() {
    xTaskCreate(myTask, "MyTask", 2048, NULL, 5, NULL);
}
```

---

## Common Tasks

### Task: Add New Menu Command

**Steps:**

1. Update `MenuSystem::handleInput()` switch case
2. Create handler function
3. Call from main state machine
4. Update help text

**Example:**

```cpp
// In menu_system.cpp
void MenuSystem::handleInput(char cmd) {
    switch(cmd) {
        case 'X':  // New command
            handleExampleCommand();
            break;
    }
}

void MenuSystem::handleExampleCommand() {
    printf("Executing example command\n");
    // Implementation
}
```

### Task: Log to SD Card

**Usage:**

```cpp
#include "sd_logger.h"

extern SDLogger logger;

// Simple log
logger.log(LOG_INFO, "System started");

// Formatted log
logger.log(LOG_BAUD, "Detected: %d bps", 115200);

// Binary data log
uint8_t data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};
logger.logData("TX", data, 5);

// Force write to disk
logger.flush();
```

### Task: Update Display

**Usage:**

```cpp
#include "display_manager.h"

extern DisplayManager display;

// Change screen
display.setState(STATE_BRIDGE_MODE);

// Update status text
display.setStatus("Connected at 115200 bps");

// Update statistics
display.updateBridgeStats(1024, 512);

// Refresh display
display.update();
```

### Task: Perform Baud Detection

**Usage:**

```cpp
#include "baud_detector.h"

extern BaudDetector baudDetector;

// Start detection
baudDetector.start();

// Poll for completion
while (!baudDetector.isComplete()) {
    vTaskDelay(pdMS_TO_TICKS(100));
}

// Get result
if (baudDetector.isSuccess()) {
    uint32_t baud = baudDetector.getDetectedBaud();
    float conf = baudDetector.getConfidence();
    printf("Detected %d bps (confidence: %.1f%%)\n", baud, conf * 100);
}
```

---

## Debugging

### Serial Monitor Output

**Start monitor:**
```bash
platformio device monitor -b 115200
```

**Control codes:**
- `Ctrl+] Q` - Quit monitor
- `Ctrl+] H` - Help

### Log Levels

Set in `platformio.ini`:

```ini
build_flags = -DCONFIG_LOG_DEFAULT_LEVEL_DEBUG=1
```

**Levels:**
- `NONE` (0) - No output
- `ERROR` (1) - Errors only
- `WARN` (2) - Warnings and errors
- `INFO` (3) - Info, warnings, errors
- `DEBUG` (4) - All including debug
- `VERBOSE` (5) - Very detailed

### Heap Monitoring

```cpp
#include "esp_heap_trace.h"

// Check free memory
size_t freeHeap = esp_get_free_heap_size();
printf("Free heap: %d bytes\n", freeHeap);

// Check minimum free heap ever
size_t minHeap = esp_get_minimum_free_heap_size();
printf("Minimum free heap: %d bytes\n", minHeap);
```

### State Machine Debugging

```cpp
void changeState(SystemState newState) {
    ESP_LOGI(TAG, "State change: %s -> %s",
             stateToString(currentState),
             stateToString(newState));
    currentState = newState;
    // ...
}

const char* stateToString(SystemState state) {
    switch(state) {
        case STATE_BOOTING: return "BOOTING";
        case STATE_WAITING: return "WAITING";
        case STATE_ANALYZING: return "ANALYZING";
        // ...
        default: return "UNKNOWN";
    }
}
```

### Device Testing

**Test hardware presence:**
```cpp
HardwareConfig hw = hwDetector.scanAll();
printf("Display: %s\n", hw.displayPresent ? "YES" : "NO");
printf("SD Card: %s\n", hw.sdCardPresent ? "YES" : "NO");
```

**Test baud detection:**
```bash
# Serial monitor window 1 (SmvIT):
# Press D to start detection

# Serial monitor window 2 (Target device):
# Run: cat /dev/ttyUSB1 (generates random data)
```

---

## Performance Tuning

### CPU Usage

Current single-threaded design runs efficiently. Use tasks if needed:

```cpp
// Task priority: 0 (lowest) to 25 (highest)
// Main work should be priority ~5
xTaskCreate(task_func, "Task", 2048, NULL, 5, NULL);
```

### Memory Optimization

- Buffer sizes in `config.h` are tuned for 320 KB RAM
- Increase cautiously
- Use `esp_get_free_heap_size()` to monitor

### Display Performance

DMA transfers handled automatically. Refresh rate can be adjusted in `display_manager.cpp`.

---

## Further Reading

- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/)
- [ESP32-C6 Datasheet](https://www.espressif.com/en/products/socs/esp32-c6/)
- [FreeRTOS Documentation](https://www.freertos.org/)
- [PlatformIO Documentation](https://docs.platformio.org/)

---

**Happy Coding! 🚀**
