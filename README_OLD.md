# SmvIT - USB-TTL Bridge with Auto Baud Detection

Professional ESP32-C6 USB-to-TTL serial bridge with intelligent baud rate detection, TFT display, SD card logging, and interactive terminal interface.

## ✨ Features

### Core Functionality
- ✅ **Intelligent Baud Detection** - GPIO interrupt-based bit timing analysis (9600-115200 bps)
- ✅ **Interactive Bridge Mode** - Transparent serial passthrough with `~~~` escape sequence
- ✅ **Professional Menu System** - ANSI-formatted terminal interface with multiple screens
- ✅ **Hardware Auto-Detection** - I2C scanning for displays, GPIO detection for SD card
- ✅ **BLE Support** - Stub implementation ready for Nordic UART Service

### Display Integration (Phase 3A) ✅
- ✅ **ILI9341 TFT Driver** - 240x320 color display with SPI at 40 MHz
- ✅ **DMA Acceleration** - Fast screen updates and smooth graphics
- ✅ **8x8 Bitmap Font** - Clear text rendering for status and data
- ✅ **Multi-Screen UI** - Boot, Menu, Analyzing, Bridge screens
- ✅ **Live Statistics** - Real-time RX/TX byte counters

### SD Card Logging (Phase 3B) ✅
- ✅ **FAT Filesystem** - Industry-standard file format
- ✅ **Timestamped Logs** - `log_YYYYMMDD_HHMMSS.txt` format
- ✅ **Structured Logging** - INFO, DATA, BAUD, STATE, TEST levels
- ✅ **Binary Data Support** - Hex dump formatting for communication data
- ✅ **Auto-Flush** - Periodic sync to prevent data loss

### Integration (Phase 4) ✅
- ✅ **Shared SPI Bus** - TFT and SD card coexist cleanly
- ✅ **Graceful Degradation** - Works without display or SD card
- ✅ **Clean Boot** - No errors with missing hardware
- ✅ **UART Bridging** - Full RX/TX passthrough with statistics

## 📋 Hardware Requirements

### Minimum Configuration
- ESP32-C6 DevKit (160MHz, 320KB RAM, 8MB Flash)
- USB-C cable for power and serial communication

### Optional Peripherals
- ILI9341 TFT Display (240x320, SPI)
- MicroSD Card Module (SPI, FAT16/FAT32)
- Target device for serial bridging

## 🔌 Hardware Connections

### ESP32-C6 Pin Assignments

#### TFT Display (ILI9341)
```
ESP32-C6      ILI9341 TFT
---------     -----------
GPIO 7    →   MOSI
GPIO 2    →   MISO
GPIO 6    →   SCLK
GPIO 10   →   CS
GPIO 3    →   DC (Data/Command)
GPIO 4    →   RST (Reset)
GPIO 5    →   BL (Backlight)
3.3V      →   VCC
GND       →   GND
```

#### SD Card Module
```
ESP32-C6      SD Card
---------     -------
GPIO 7    →   MOSI (shared with TFT)
GPIO 2    →   MISO (shared with TFT)
GPIO 6    →   SCLK (shared with TFT)
GPIO 10   →   CS (separate device)
3.3V      →   VCC
GND       →   GND
```

#### UART Bridge (Target Device)
```
ESP32-C6      Target
---------     ------
GPIO 16   →   TX (connect to target's RX)
GPIO 17   →   RX (connect to target's TX)
GND       →   GND (common ground required)
```

#### I2C (Optional - for other displays)
```
ESP32-C6      I2C Device
---------     ----------
GPIO 8    →   SDA
GPIO 9    →   SCL
3.3V      →   VCC
GND       →   GND
```

## 🚀 Quick Start

### Building & Flashing

```bash
# Install PlatformIO if not already installed
pip install platformio

# Clone the repository
git clone <repository-url>
cd SmvIT

# Build the project
pio run --environment esp32c6

# Upload to ESP32-C6
pio run -t upload --environment esp32c6

# Upload and monitor serial output
pio run -t upload -t monitor --environment esp32c6
```

### First Boot

1. Connect ESP32-C6 via USB
2. Upload firmware
3. Open serial monitor at 115200 baud
4. You'll see the SmvIT menu system

### Menu Commands

```
╔════════════════════════════════════════╗
║            SmvIT Main Menu             ║
╚════════════════════════════════════════╝

  D - Detect Hardware    Scan for TFT & SD card
  B - Bridge Mode        Enter transparent passthrough
  S - Settings           Configure baud rate
  I - Info               Show hardware status
  H - Help               Display this menu
  R - Reset              Restart device

Status: Ready
Hardware: Display [N/A] | SD Card [N/A]
```

### Using Bridge Mode

1. Type `b` to enter bridge mode
2. Connect target device to GPIO16/17
3. All data passes through transparently
4. Type `~~~` (three tildes) to exit back to menu
5. View statistics with `i` command

## 📊 System States

### State Machine
```
BOOTING       →  System initialization
MENU          →  Command interface (default)
ANALYZING     →  Baud rate detection in progress
BRIDGE_MODE   →  Transparent data passthrough
```

### Workflow Example
```
1. Boot → Menu
2. Press 'd' → Detect hardware
3. Connect target device
4. Press 'b' → Enter bridge mode
5. Data flows through automatically
6. Logs saved to SD card
7. Type '~~~' → Return to menu
```
6. **RESTART_NEEDED** - Vyžaduje reštart cieľového zariadenia
7. **RUNNING** - Monitruje aktívnu komunikáciu

## Použitie

1. Zapnite ESP32-C6
2. Na displeji uvidíte aktuálny stav
3. Pripojte cieľové zariadenie na RX/TX
4. Systém automaticky deteguje baud rate
5. Všetky dáta sa ukladajú na SD kartu
6. Na displeji vidíte priebežný stav

## Logovanie

Všetky správy a dáta sa ukladajú na SD kartu v súbore:
`LOG_[timestamp].txt`

Formát logu:
```
[timestamp] [counter] MESSAGE
[timestamp] [counter] RX DATA: ...
[timestamp] [counter] BAUD DETECTED: 115200 bps
```

## Konfigurácia v config.h

```cpp
#define RX_PIN 4
#define TX_PIN 5
#define SD_CS 9
#define TFT_CS 10
// ... viac konfigurácie
```

## Príklady použitia

### Testovanie komunikácie

Systém pošle testovacie hlásenie "AT\r\n" a očakáva odpoveď.

### Monitorovanie dát

Všetky RX dáta sa zobrazujú na displeji a ukladajú na SD kartu.

## Podpora

Pre otázky a problémy, pozrite sa na dokumentáciu v PDF súbore.

## Licencia

MIT License


## 📁 Project Structure

```
SmvIT/
├── include/                        # Header files
│   ├── config.h                    # System-wide configuration
│   ├── hardware_detector.h         # I2C/GPIO hardware detection
│   ├── baud_detector.h             # Intelligent baud rate detection
│   ├── bridge_mode.h               # Transparent passthrough mode
│   ├── menu_system.h               # ANSI terminal interface
│   ├── tft_driver.h                # ILI9341 display driver
│   ├── display_manager.h           # Display abstraction layer
│   ├── sd_logger.h                 # SD card logging system
│   └── bluetooth_manager.h         # BLE stub (future)
├── src/                            # Source files
│   ├── main.cpp                    # Entry point & state machine
│   ├── hardware_detector.cpp       # Hardware scanning
│   ├── baud_detector_espidf.cpp    # GPIO ISR baud detection
│   ├── bridge_mode.cpp             # Data forwarding
│   ├── menu_system.cpp             # Menu rendering
│   ├── tft_driver.cpp              # SPI TFT graphics
│   ├── display_manager_espidf.cpp  # Display screens
│   └── sd_logger_espidf.cpp        # FAT filesystem logging
├── INTEGRATION_TEST_PLAN.md        # Test cases
├── PHASE_3_4_COMPLETE.md           # Implementation summary
└── integration_test.py             # Automated tests
```

## 📝 Development Status

### ✅ Completed
- Phase 1-4: Full system implementation
- Hardware auto-detection, baud detection, bridge mode
- Menu system, display driver, SD card logging
- Integration testing and fixes

### 🔄 Ready for Testing
- Physical TFT display integration
- SD card hardware testing

### 📋 Planned (Phase 5)
- BLE Nordic UART Service
- Mobile app with Flipper Zero-style UI
- Configuration persistence
- OTA firmware updates

## 📚 Documentation

- `BUILD_GUIDE.md` - Build instructions
- `LINUX_SETUP.md` - Linux setup guide
- `INTEGRATION_TEST_PLAN.md` - Test procedures
- `PHASE_3_4_COMPLETE.md` - Implementation details

---

**Version**: SmvIT-0.1.0  
**Updated**: December 8, 2025  
**Status**: Phase 3 & 4 Complete ✅
