# 🔌 SerialyTTY - Professional USB-TTL Bridge with Auto Baud Detection

> An intelligent ESP32-C6 USB-to-TTL serial bridge with automatic baud rate detection, TFT display support, SD card logging, and interactive terminal interface.

**Status:** ✅ Production Ready (Phase 4 Complete)  
**License:** Dual License (MIT for code, CC-BY-4.0 for documentation)  
**Repository:** [thenisvan/SerialyTTY](https://github.com/thenisvan/SerialyTTY)  
**Documentation:** [SerialyTTY Docs](https://thenisvan.github.io/SerialyTTY/)  
**Last Updated:** December 8, 2025

---

## 🎯 Overview

SerialyTTY is a professional-grade USB-to-TTL serial bridge device built on ESP32-C6 that intelligently detects baud rates, bridges serial communication, and provides comprehensive logging and monitoring capabilities. It's designed for developers, embedded engineers, and hardware testers who need reliable serial communication diagnostics.

### 🌟 Key Features

#### Core Bridge Functionality
- **Intelligent Baud Detection** - Automatic detection from 9600 to 115200 bps using GPIO interrupt-based bit timing analysis
- **Transparent Serial Bridging** - Real-time transparent passthrough with `~~~` escape sequence for mode switching
- **Hardware Auto-Detection** - Automatic scanning for optional peripherals (display, SD card)
- **Graceful Degradation** - Works seamlessly with or without optional hardware

#### Display & Monitoring
- **ILI9341 TFT Integration** - 240x320 color display with DMA-accelerated graphics
- **Real-Time Statistics** - Live RX/TX byte counters and throughput display
- **Multi-Screen UI** - Boot, Menu, Analyzing, and Bridge screens with professional layout
- **Status Indicators** - Connection status, baud rate, and hardware availability

#### Data Logging & Storage
- **SD Card Support** - FAT filesystem with automatic directory management
- **Timestamped Logs** - `log_YYYYMMDD_HHMMSS.txt` format with structured entries
- **Multiple Log Levels** - INFO, DATA, BAUD, STATE, TEST with binary data support
- **Hex Dump Formatting** - Automatic formatting of binary data for easy analysis
- **Auto-Flush** - Periodic sync to prevent data loss

#### Interactive Interface
- **Professional Menu System** - ANSI-formatted terminal with command-line interface
- **Multiple Modes** - Menu, Bridge, Testing, and Configuration modes
- **Hardware Information** - Real-time display of detected hardware and status
- **Communication Testing** - Built-in communication tester for device verification

#### Connectivity
- **BLE Support** - Bluetooth Low Energy stub implementation (Nordic UART Service ready)
- **USB Serial** - Full USB-C serial communication at 115200 bps

---

## 📋 Hardware Requirements

### Essential Hardware
- **ESP32-C6 DevKit** - Minimum: 160 MHz CPU, 320 KB RAM, 8 MB Flash
- **USB-C Cable** - For power and serial communication

### Optional Peripherals
- **ILI9341 TFT Display** - 240x320, SPI interface (recommended for enhanced usability)
- **MicroSD Card Module** - SPI interface with FAT16/FAT32 support (recommended for logging)
- **Target Device** - Any device with UART for bridge testing

---

## 🔌 Hardware Connections

### ESP32-C6 Pin Assignments

#### USB-TTL Bridge UART (UART1)
```
RX:  GPIO16 (UART1_RXD) → Target device TX
TX:  GPIO17 (UART1_TXD) → Target device RX
GND: GND                 → Target device GND
```

#### TFT Display (ILI9341) - SPI2
```
MOSI:  GPIO7   → D7 (SDA / MOSI)
MISO:  GPIO2   → D6 (MISO)
SCLK:  GPIO6   → D8 (SCL / SCLK)
CS:    GPIO10  → CS (Chip Select)
DC:    GPIO3   → RS (Data/Command)
RST:   GPIO4   → RESET
BL:    GPIO5   → Backlight (PWM)
```

#### SD Card Module - SPI2 (Shared with Display)
```
MOSI:  GPIO7   → DIN (Shared with TFT)
MISO:  GPIO2   → DO (Shared with TFT)
SCLK:  GPIO6   → CLK (Shared with TFT)
CS:    GPIO1   → CS (SD Card Select)
```

#### Hardware Detection (I2C)
```
SDA: GPIO19 → I2C SDA (for display detection)
SCL: GPIO20 → I2C SCL (for display detection)
```

---

## 🚀 Quick Start

### Prerequisites
- PlatformIO CLI or VS Code + PlatformIO Extension
- ESP32-C6 DevKit connected via USB

### Build & Upload (5 minutes)

#### Option 1: Using PlatformIO CLI
```bash
# Clone repository
git clone https://github.com/thenisvan/SerialyTTY.git
cd SerialyTTY

# Build for ESP32-C6
platformio run -e esp32c6

# Upload to device
platformio run -e esp32c6 -t upload

# Monitor serial output
platformio device monitor -b 115200
```

#### Option 2: Using VS Code
1. Install PlatformIO IDE extension
2. Open project folder
3. Click "Build" (✓) icon
4. Click "Upload" (→) icon
5. Click "Serial Monitor" (⊔) icon

### Expected Boot Sequence
```
I (2330) MAIN: === USB-TTL SNIFFER STARTING ===
I (2340) MAIN: USB Serial initialized
I (2350) HW_DETECT: Scanning for hardware modules...
I (2360) DISPLAY: Display initialization (optional)
I (2530) SD_LOGGER: SD card initialization (optional)
I (3500) BAUD_DETECT: Baud detector initialized
I (4460) BLE_UART: BLE stub initialized
I (5920) DISPLAY: State changed to: Menu
```

---

## 📖 Documentation Structure

```
📁 SerialyTTY/
├── 📄 README.md                    (This file)
├── 📄 USER_GUIDE.md                Complete usage manual
├── 📄 API_DOCUMENTATION.md         Developer API reference
├── 📄 FAQ_TROUBLESHOOTING.md       Common issues and solutions
├── 📄 BUILD_GUIDE.md               Detailed build instructions
├── 📄 LINUX_SETUP.md               Linux-specific setup
├── 📄 PHASE_3_4_COMPLETE.md        Implementation details
├── 📄 INTEGRATION_TEST_PLAN.md     Testing procedures
├── 📁 include/                     Header files (public API)
├── 📁 src/                         Source code (implementations)
├── 📄 platformio.ini               Build configuration
├── 📄 CMakeLists.txt               CMake build config
└── 📄 sdkconfig.esp32c6            ESP-IDF configuration
```

---

## 💡 Usage Scenarios

### Scenario 1: Finding Unknown Baud Rate
1. Connect target device to UART1 pins
2. Power on SmvIT
3. Device automatically detects baud rate
4. Bridge mode activates automatically
5. View detection result on display or serial output

### Scenario 2: Logging Communication Data
1. Insert microSD card into module
2. Power on SmvIT
3. All communication is automatically logged to SD card
4. Access logs via computer (FAT filesystem format)
5. Analyze binary data in log files

### Scenario 3: Interactive Terminal Session
1. Connect to SmvIT via USB
2. Open serial terminal at 115200 bps
3. Navigate menu with commands: D, B, S, I, H
4. View live statistics and hardware information
5. Switch between modes as needed

---

## 🏗️ Architecture Overview

### System Components

```
┌─────────────────────────────────────────────────────────┐
│                   USB-TTL Bridge (ESP32-C6)             │
├─────────────────────────────────────────────────────────┤
│                                                           │
│  ┌──────────────────────────────────────────────────┐   │
│  │           Hardware Detection Module              │   │
│  │  • I2C Scanner (Display Detection)               │   │
│  │  • GPIO Detection (SD Card)                      │   │
│  └──────────────────────────────────────────────────┘   │
│                           ▼                              │
│  ┌──────────────────────────────────────────────────┐   │
│  │          State Machine & Bridge Mode             │   │
│  │  • BOOTING → WAITING → ANALYZING                │   │
│  │  → FOUND_SPEED → RUNNING → BRIDGE_MODE          │   │
│  └──────────────────────────────────────────────────┘   │
│          ▲                   ▼                    ▲       │
│  ┌───────┴──────────┐  ┌────────────┐  ┌────────┴───┐   │
│  │  Baud Detector   │  │  UART1     │  │  BLE UART  │   │
│  │  (GPIO Timing)   │  │  (Bridge)  │  │  (Nordic)  │   │
│  └──────────────────┘  └────────────┘  └────────────┘   │
│          ▼                                        ▼       │
│  ┌─────────────────────────────────────────────────┐    │
│  │    Display Manager (ILI9341 TFT - Optional)    │    │
│  │  • Boot, Menu, Analyzing, Bridge screens       │    │
│  └─────────────────────────────────────────────────┘    │
│          ▼                                               │
│  ┌─────────────────────────────────────────────────┐    │
│  │    SD Logger (FAT Filesystem - Optional)        │    │
│  │  • Timestamped logs with hex dumps              │    │
│  └─────────────────────────────────────────────────┘    │
│                                                           │
│  ┌─────────────────────────────────────────────────┐    │
│  │    Menu System & Communication Tester           │    │
│  │  • Interactive terminal interface               │    │
│  └─────────────────────────────────────────────────┘    │
│                                                           │
└─────────────────────────────────────────────────────────┘
```

### Key Modules

| Module | File | Purpose |
|--------|------|---------|
| **Baud Detector** | `baud_detector.cpp` | GPIO interrupt-based baud rate detection |
| **UART Bridge** | `bridge_mode.cpp` | Transparent serial passthrough |
| **Display Manager** | `display_manager.cpp` | TFT control and screen management |
| **SD Logger** | `sd_logger.cpp` | SD card initialization and logging |
| **Menu System** | `menu_system.cpp` | Terminal-based interactive menu |
| **Hardware Detector** | `hardware_detector.cpp` | Peripheral scanning and detection |
| **BLE Manager** | `bluetooth_manager.cpp` | Bluetooth Low Energy stub |
| **Comm Tester** | `comm_tester.cpp` | Communication testing utilities |

---

## 🔄 Development Workflow

### Branch Structure
- **`main`** - Production-ready code, stable releases
- **`dev`** - Development branch, new features, testing

### Making Changes
```bash
# Clone and switch to dev branch
git clone https://github.com/thenisvan/SerialyTTY.git
cd SerialyTTY
git checkout -b dev origin/dev

# Make your changes
# Test locally
platformio run -e esp32c6
platformio run -e esp32c6 -t upload

# Commit and push
git add .
git commit -m "feature: description of changes"
git push origin dev

# Create Pull Request to main for review
```

---

## 📊 Performance Specifications

| Feature | Value |
|---------|-------|
| **CPU Speed** | 160 MHz |
| **RAM Available** | ~200 KB (after system) |
| **Flash Storage** | 8 MB |
| **Baud Rate Range** | 9600 - 115200 bps |
| **Detection Speed** | ~2-5 seconds |
| **RX Buffer Size** | 1 KB |
| **TX Buffer Size** | 1 KB |
| **Display Refresh Rate** | 20 Hz (50 ms) |
| **Log Flush Interval** | Every 10 entries |
| **Max Log File Size** | Limited by SD card |

---

## 🐛 Troubleshooting

### Device Not Detected
- Ensure USB-C cable is properly connected
- Try different USB port or cable
- Check device manager for serial port

### Baud Detection Fails
- Ensure target device is properly connected
- Check that target device is sending data continuously
- Verify power supply to target device

### SD Card Not Recognized
- Format SD card as FAT16 or FAT32 first
- Check CS pin connection (GPIO1)
- Ensure SPI bus is not already claimed

### Display Shows Garbage
- Verify display connections (especially DC and RST pins)
- Check display is connected to I2C for proper detection
- Try disabling display with `tryDisplay = false` in code

See **FAQ_TROUBLESHOOTING.md** for more detailed solutions.

---

## 📚 Additional Resources

- 📖 [User Guide](./USER_GUIDE.md) - Complete usage manual
- 🔧 [API Documentation](./API_DOCUMENTATION.md) - Developer reference
- ❓ [FAQ & Troubleshooting](./FAQ_TROUBLESHOOTING.md) - Common issues
- 🏗️ [Build Guide](./BUILD_GUIDE.md) - Detailed build instructions
- 🧪 [Integration Tests](./INTEGRATION_TEST_PLAN.md) - Testing procedures
- 💻 [Linux Setup](./LINUX_SETUP.md) - Linux-specific instructions

---

## 📝 License

This project uses a **dual license** approach:

### Code License (MIT)
All source code (`.cpp`, `.h`, `.py`, configuration files) is licensed under the [MIT License](./LICENSE).

```
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software...
```

### Documentation License (CC-BY-4.0)
All documentation and educational content (`.md` files, guides, methodologies) is licensed under the [Creative Commons Attribution 4.0 International License](./LICENSE-DOCS).

```
You are free to:
• Share — copy and redistribute the material
• Adapt — remix, transform, and build upon the material
Required: Attribution — You must give appropriate credit
```

---

## 🤝 Contributing

We welcome contributions! Here's how to get involved:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Make** your changes and test thoroughly
4. **Commit** with clear messages (`git commit -m 'Add amazing feature'`)
5. **Push** to your branch (`git push origin feature/amazing-feature`)
6. **Open** a Pull Request with a clear description

### Guidelines
- Follow the existing code style
- Add tests for new features
- Update documentation as needed
- Run security checks: `snyk code scan`
- Ensure all tests pass

---

## 🏆 Project Status

### ✅ Completed Phases

| Phase | Feature | Status |
|-------|---------|--------|
| **Phase 1** | Core Bridge & Baud Detection | ✅ Complete |
| **Phase 2** | Menu System & Comm Testing | ✅ Complete |
| **Phase 3A** | TFT Display Integration | ✅ Complete |
| **Phase 3B** | SD Card Logging | ✅ Complete |
| **Phase 4** | Integration & Testing | ✅ Complete |

### 🔮 Future Phases (Planned)

| Phase | Focus |
|-------|-------|
| **Phase 5** | Advanced Protocol Analysis (I2C, SPI, CAN) |
| **Phase 6** | Real-Time Data Visualization |
| **Phase 7** | Mobile App Integration |
| **Phase 8** | Cloud Logging & Analytics |

---

## 📞 Support & Community

- **Issues & Bugs:** [GitHub Issues](https://github.com/thenisvan/SerialyTTY/issues)
- **Discussions:** [GitHub Discussions](https://github.com/thenisvan/SerialyTTY/discussions)
- **Documentation:** See `/docs/` folder for comprehensive guides

---

## 👨‍💻 Author

**Created by:** Denis Nisan (thenisvan)  
**Organization:** SmvIT Development  
**Last Updated:** December 8, 2025

---

## 🙏 Acknowledgments

- ESP-IDF Community for excellent documentation
- PlatformIO for streamlined development
- All contributors and testers who helped shape SmvIT

---

**Happy Bridging! 🚀**

---

*For more information, visit the complete documentation in the `/docs/` folder.*
