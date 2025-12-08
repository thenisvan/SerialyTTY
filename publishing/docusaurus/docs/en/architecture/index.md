---
id: architecture
title: Software Architecture
sidebar_position: 4
---

# Software Architecture

Understanding the internal design and organization of SerialyTTY.

## System Overview

SerialyTTY is built on a modular architecture using ESP-IDF framework and FreeRTOS.

```
┌─────────────────────────────────────────────────┐
│              Application Layer                   │
│  ┌──────────┐  ┌──────────┐  ┌──────────────┐  │
│  │   Menu   │  │  Bridge  │  │ Baud Detect  │  │
│  │  System  │  │   Mode   │  │              │  │
│  └──────────┘  └──────────┘  └──────────────┘  │
├─────────────────────────────────────────────────┤
│              Hardware Abstraction                │
│  ┌──────────┐  ┌──────────┐  ┌──────────────┐  │
│  │ Display  │  │   SD     │  │   Hardware   │  │
│  │ Manager  │  │  Logger  │  │   Detector   │  │
│  └──────────┘  └──────────┘  └──────────────┘  │
├─────────────────────────────────────────────────┤
│              Driver Layer (ESP-IDF)              │
│  ┌──────────┐  ┌──────────┐  ┌──────────────┐  │
│  │   UART   │  │   SPI    │  │     I2C      │  │
│  │  Driver  │  │  Driver  │  │    Driver    │  │
│  └──────────┘  └──────────┘  └──────────────┘  │
├─────────────────────────────────────────────────┤
│                FreeRTOS Kernel                   │
└─────────────────────────────────────────────────┘
```

## Module Breakdown

### Main Application (`main.cpp`)

Central orchestrator managing system state and module coordination.

**Responsibilities:**
- System initialization
- State machine management
- Module lifecycle control
- Main event loop

**State Machine:**
```cpp
enum SystemState {
    STATE_BOOTING,      // Initialization
    STATE_MENU,         // User interaction
    STATE_ANALYZING,    // Baud detection active
    STATE_FOUND_SPEED,  // Detection complete
    STATE_BRIDGE,       // Passthrough mode
    STATE_TESTING       // Communication test
};
```

### Hardware Detector (`hardware_detector.h/cpp`)

Discovers and validates connected peripherals.

```cpp
class HardwareDetector {
public:
    HardwareConfig scanAll();
    bool detectDisplay();
    bool detectSDCard();
    bool detectI2CDevices();
private:
    I2CScanner i2cScanner;
    GPIOProbe gpioProbe;
};
```

**Detection Process:**
1. I2C bus scan (0x08-0x77)
2. GPIO probe for CS lines
3. SPI initialization tests
4. Configuration object creation

### Display Manager (`display_manager.h/cpp`)

Abstracts TFT display operations with state-aware rendering.

```cpp
class DisplayManager {
public:
    bool begin();
    void setState(SystemState state);
    void setStatus(const char* status);
    void update();
    void showStats(uint32_t rxBytes, uint32_t txBytes);
private:
    TFTDriver tft;
    SystemState currentState;
    void renderBootScreen();
    void renderMenuScreen();
    void renderBridgeScreen();
};
```

**Rendering Pipeline:**
1. State change detection
2. Screen buffer preparation
3. DMA transfer initiation
4. Frame update completion

### Baud Detector (`baud_detector.h/cpp`)

Interrupt-driven timing analysis for baud rate detection.

```cpp
class BaudDetector {
public:
    void start();
    void stop();
    uint32_t getDetectedBaud();
    bool isDetectionComplete();
private:
    static void IRAM_ATTR rxISR(void* arg);
    void analyzeTiming();
    uint32_t calculateBaud();
    
    std::vector<uint32_t> timingSamples;
    volatile bool detecting;
};
```

**Algorithm:**
1. Configure GPIO interrupt (falling edge)
2. Capture edge timestamps (ESP timer)
3. Calculate bit periods
4. Match to standard baud rates
5. Verify consistency

### SD Logger (`sd_logger_espidf.h/cpp`)

VFS-based filesystem logging with structured format.

```cpp
class SDLogger {
public:
    bool begin();
    void log(LogLevel level, const char* message);
    void logData(const char* prefix, uint8_t* data, size_t len);
    void flush();
private:
    FILE* logFile;
    uint32_t entryCount;
    std::string formatTimestamp();
    std::string formatHexDump(uint8_t* data, size_t len);
};
```

**File Operations:**
- Mount: `/sdcard` mount point
- Path: `/sdcard/logs/log_YYYYMMDD_HHMMSS.txt`
- Write: Buffered with periodic flush
- Close: Safe unmount on shutdown

### Bridge Mode (`bridge_mode.h/cpp`)

Bidirectional UART-to-USB passthrough.

```cpp
class BridgeMode {
public:
    void start(uint32_t baudRate);
    void stop();
    void process();
    uint32_t getRxBytes();
    uint32_t getTxBytes();
private:
    void forwardUART0ToUART1();
    void forwardUART1ToUART0();
    bool checkEscapeSequence();
    
    uint32_t rxCounter;
    uint32_t txCounter;
    char escapeBuffer[4];
};
```

**Data Flow:**
```
USB (UART0) ←→ Bridge Controller ←→ UART1 (Target)
     ↕                                    ↕
  Statistics                         Statistics
```

### Menu System (`menu_system.h/cpp`)

ANSI terminal UI with command processing.

```cpp
class MenuSystem {
public:
    void show();
    void handleInput(char cmd);
    void printHelp();
    void printInfo();
private:
    void clearScreen();
    void printHeader();
    void printCommands();
    
    static const char* ANSI_CLEAR;
    static const char* ANSI_BOLD;
    static const char* ANSI_RESET;
};
```

**ANSI Codes:**
- `\033[2J` - Clear screen
- `\033[H` - Cursor home
- `\033[1m` - Bold text
- `\033[0m` - Reset formatting

## Data Flow

### Boot Sequence

```
Power On
   ↓
Initialize FreeRTOS
   ↓
Setup USB Serial (UART0)
   ↓
Scan Hardware (I2C, GPIO)
   ↓
Initialize Display (if present)
   ↓
Mount SD Card (if present)
   ↓
Initialize UART1
   ↓
Setup Baud Detector
   ↓
Show Menu
```

### Baud Detection Flow

```
User presses 'D'
   ↓
Enter ANALYZING state
   ↓
Configure GPIO16 interrupt
   ↓
Wait for data... (interrupt fires)
   ↓
Capture timestamps
   ↓
Calculate bit periods
   ↓
Match to standard bauds
   ↓
Display result
   ↓
Enter FOUND_SPEED state
```

### Bridge Mode Flow

```
User presses 'B'
   ↓
Configure UART1 (detected baud)
   ↓
Enter BRIDGE state
   ↓
Loop:
  ├─ Read UART0 → Write UART1
  ├─ Read UART1 → Write UART0
  ├─ Update counters
  ├─ Check escape sequence
  └─ Update display
   ↓
Escape detected ('~~~')
   ↓
Return to MENU state
```

## Memory Management

### Static Allocation

**Advantages:**
- Deterministic behavior
- No fragmentation
- Fast allocation

**Usage:**
```cpp
static uint8_t displayBuffer[76800];  // 240x320 RGB565
static char logBuffer[512];           // SD card writes
static uint8_t uartRxBuffer[1024];    // UART receive
```

### Heap Usage

**ESP32-C6 Memory Map:**
- Total RAM: 320 KB
- FreeRTOS: ~20 KB
- WiFi/BLE: ~40 KB (if enabled)
- Application: ~260 KB available

**Optimization:**
- Use `IRAM_ATTR` for ISRs
- DMA buffers in DMA-capable RAM
- Stack size tuning per task

## Interrupt Handling

### Priority Levels

| Interrupt | Priority | Notes |
|-----------|----------|-------|
| GPIO (Baud Detect) | 3 | High - timing critical |
| UART RX | 2 | Medium - data critical |
| SPI (DMA) | 1 | Low - background |
| Timer | 2 | Medium - scheduling |

### ISR Best Practices

```cpp
void IRAM_ATTR rxISR(void* arg) {
    // MINIMAL code in ISR
    uint32_t timestamp = esp_timer_get_time();
    xQueueSendFromISR(queue, &timestamp, NULL);
    // Processing in task context
}
```

## Threading Model

### FreeRTOS Tasks

```cpp
// Main task (default)
void app_main(void) {
    // Priority: 1
    // Stack: 8KB
    setup_hardware();
    while(1) {
        process_state_machine();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Display update task (future)
void display_task(void* arg) {
    // Priority: 2
    // Stack: 4KB
    while(1) {
        update_display();
        vTaskDelay(pdMS_TO_TICKS(33)); // 30 FPS
    }
}
```

### Synchronization

- **Mutex**: SPI bus access (display vs SD)
- **Semaphore**: UART buffer ready
- **Queue**: ISR to task communication
- **Event Group**: State transitions

## Configuration System

### Compile-Time (`config.h`)

```cpp
#define MIN_BAUD 9600
#define MAX_BAUD 115200
#define SPI_FREQ_MHZ 40
#define SD_MOUNT_POINT "/sdcard"
#define LOG_BUFFER_SIZE 512
```

### Runtime (`sdkconfig`)

ESP-IDF menuconfig options:
- FreeRTOS tick rate
- UART buffer sizes
- SPI DMA channels
- Logging levels

## Error Handling Strategy

### Graceful Degradation

```cpp
if (!display.begin()) {
    ESP_LOGW(TAG, "Display not available");
    // Continue without display
}

if (!logger.begin()) {
    ESP_LOGW(TAG, "SD logging disabled");
    // Continue without logging
}
```

### Error Recovery

```cpp
esp_err_t err = uart_driver_install(...);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "UART init failed: %s", 
             esp_err_to_name(err));
    return false;
}
```

## Performance Metrics

### Typical Operation

- **Boot time**: ~3 seconds
- **Baud detection**: <1 second
- **Bridge latency**: <1 ms
- **Display update**: 30 FPS
- **SD write speed**: ~100 KB/s
- **CPU utilization**: 15-25%

### Optimization Techniques

1. **DMA for bulk transfers**
2. **Interrupt-driven I/O**
3. **Minimal ISR code**
4. **Efficient state machine**
5. **Buffered logging**

---

**Next:** Learn about [Testing](../testing/index.md) procedures and validation.
