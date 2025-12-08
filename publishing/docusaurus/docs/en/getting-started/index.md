---
id: getting-started
title: Getting Started
sidebar_position: 1
---

# Getting Started with SerialyTTY

This guide will help you set up and start using SerialyTTY for the first time.

## Prerequisites

### Required Hardware
- ESP32-C6 DevKit board
- USB-C cable for power and programming
- Computer with USB port (macOS, Linux, or Windows)

### Optional Hardware
- ILI9341 TFT Display (240x320, SPI)
- MicroSD Card Module (SPI)
- Target device with UART interface for bridging

### Software Requirements
- [PlatformIO IDE](https://platformio.org/) or PlatformIO Core
- VS Code (recommended) or compatible IDE
- USB serial terminal (built-in or external)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/thenisvan/SerialyTTY.git
cd SerialyTTY
```

### 2. Install Dependencies

PlatformIO will automatically install the required dependencies when you first build the project.

### 3. Build the Firmware

For ESP32-C6:
```bash
platformio run -e esp32c6
```

For ESP32 (standard):
```bash
platformio run -e esp32dev
```

### 4. Upload to Device

Connect your ESP32-C6 board via USB and run:

```bash
platformio run -e esp32c6 -t upload
```

### 5. Monitor Serial Output

```bash
platformio device monitor -b 115200
```

Or use the PlatformIO IDE Serial Monitor.

## First Boot

When you first power on SerialyTTY, you'll see:

1. **Boot Sequence** - System initialization messages
2. **Hardware Detection** - Scanning for connected peripherals
3. **Main Menu** - Interactive command interface

### Expected Output

```
I (2330) MAIN: === USB-TTL SNIFFER STARTING ===
I (2340) MAIN: USB Serial initialized
I (2350) HW_DETECT: Scanning for hardware modules...
I (2360) DISPLAY: Display initialization skipped (not connected)
I (2530) SD_LOGGER: SD card not detected
I (3500) BAUD_DETECT: Baud detector initialized
I (4460) BLE_UART: BLE stub initialized
```

## Basic Usage

### Accessing the Menu

Press any key in the serial terminal to access the main menu:

```
=== MAIN MENU ===
[D] Detect Baud Rate
[B] Bridge Mode
[S] Settings
[I] Info
[H] Help
```

### Detecting Baud Rate

1. Press `D` to enter detection mode
2. Connect your target device to UART1 (GPIO16/17)
3. Have the target device send data
4. SerialyTTY will automatically detect the baud rate

### Bridge Mode

1. Press `B` to enter bridge mode
2. Data flows transparently between USB and UART1
3. Press `~~~` (three tildes) to exit back to menu

### Viewing System Info

Press `I` to see:
- Hardware configuration
- Detected peripherals
- Firmware version
- Memory usage

## Hardware Connections

### Minimal Setup (UART Bridge Only)

Connect your target device to:
- **RX (GPIO16)** ← TX from target
- **TX (GPIO17)** → RX from target
- **GND** ← GND from target

### With TFT Display (Optional)

Connect ILI9341 display:
- **MOSI** → GPIO7
- **MISO** → GPIO2
- **SCLK** → GPIO6
- **CS** → GPIO10
- **DC** → GPIO3
- **RST** → GPIO4
- **BL** → GPIO5
- **VCC** → 3.3V
- **GND** → GND

### With SD Card (Optional)

Connect SD card module:
- **MOSI** → GPIO7 (shared with display)
- **MISO** → GPIO2 (shared with display)
- **SCLK** → GPIO6 (shared with display)
- **CS** → GPIO1
- **VCC** → 3.3V
- **GND** → GND

## Configuration

### Enabling Display

In `src/main.cpp`, change:

```cpp
bool tryDisplay = false;  // Default: disabled
```

To:

```cpp
bool tryDisplay = true;   // Enable display support
```

### Adjusting Baud Rate Range

In `include/config.h`:

```cpp
#define MIN_BAUD 9600
#define MAX_BAUD 115200
```

## Troubleshooting

### No Serial Output
- Check USB cable connection
- Verify correct COM port selection
- Try resetting the board (EN button)

### Display Not Working
- Verify connections match pin configuration
- Check that `tryDisplay = true` in main.cpp
- Ensure 3.3V power is stable

### SD Card Not Detected
- Format card as FAT32
- Check SPI connections
- Verify CS pin (GPIO1)
- Try a different SD card

### Build Errors on macOS ARM64

ESP32-C6 may have limited support on Apple Silicon. Solutions:
- Use Docker with x64 emulation (see BUILD_GUIDE.md)
- Use standard ESP32 (`esp32dev` environment)
- Build on Linux/Windows x64 system

## Next Steps

- Read the [Hardware Guide](../hardware/index.md) for detailed pin configurations
- Explore [Features](../features/index.md) to learn about all capabilities
- Check [Testing Guide](../testing/index.md) for validation procedures
- Review [Architecture](../architecture/index.md) to understand the system design

## Getting Help

- 📖 Check the full documentation
- 🐛 Report issues on GitHub
- 💬 Ask questions in Discussions
- 📧 Contact the maintainers

---

**Ready to start?** Connect your ESP32-C6, upload the firmware, and open the serial monitor! 🚀
