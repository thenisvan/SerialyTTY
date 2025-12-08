---
fm_version: "1.0.0"
fm_build: "2025-12-08T00:00:00Z"
id: SERIALYTTY_HOME
guid: "a1b2c3d4-e5f6-7890-abcd-ef1234567890"
dao: "serialytty"
title: "SerialyTTY - USB-TTL Bridge Documentation"
description: "Professional ESP32-C6 USB-to-TTL serial bridge with intelligent baud rate detection, TFT display, SD card logging, and interactive terminal interface."
author: "Denis"
category: "hardware"
type: "documentation"
tags: ["esp32", "uart", "serial", "bridge", "embedded"]
locale: "en"
created: "2025-12-08"
modified: "2025-12-08"
status: "active"
privacy: "public"
license: "MIT"
slug: /
sidebar_position: 1
---

# SerialyTTY Documentation

> **Professional ESP32-C6 USB-to-TTL Serial Bridge**  
> Intelligent baud rate detection • TFT display • SD card logging • Bridge mode

## 🚀 Quick Navigation

- 📖 [**Getting Started**](./en/getting-started/index.md) - Setup and first steps
- 🔧 [**Hardware Guide**](./en/hardware/index.md) - Pin configurations and wiring
- 📚 [**Features**](./en/features/index.md) - Core functionality overview
- 💻 [**Software Architecture**](./en/architecture/index.md) - System design and modules
<!-- - 🧪 [**Testing & Integration**](./en/testing/index.md) - Test plans and validation -->
<!-- - 📚 [**API Reference**](./en/api/index.md) - Code documentation -->

## 🎯 Project Overview

**SerialyTTY** (SmvIT) is a professional-grade USB-to-TTL serial bridge built on the ESP32-C6 platform. It combines intelligent hardware detection, real-time baud rate analysis, and comprehensive logging capabilities into a single, elegant solution for embedded systems development and debugging.

### Key Capabilities

✅ **Intelligent Baud Detection** - Automatically detects communication speed (9600-115200 bps) using GPIO interrupt-based bit timing analysis

✅ **Interactive Bridge Mode** - Transparent serial passthrough with escape sequences for control

✅ **Professional Menu System** - ANSI-formatted terminal interface with multiple screens and navigation

✅ **TFT Display Integration** - 240x320 ILI9341 display with DMA acceleration and live statistics

✅ **SD Card Logging** - Timestamped logs with structured data and hex dump support

✅ **Hardware Auto-Detection** - I2C scanning for displays, GPIO detection for peripherals

✅ **Graceful Degradation** - Works seamlessly with or without optional peripherals

## 🏗️ Development Status

| Phase | Feature | Status |
|-------|---------|--------|
| Phase 1 | Core UART Bridge | ✅ Complete |
| Phase 2 | Baud Detection & Menu | ✅ Complete |
| Phase 3A | TFT Display Integration | ✅ Complete |
| Phase 3B | SD Card Logging | ✅ Complete |
| Phase 4 | Integration & Testing | ✅ Complete |
| Phase 5 | Advanced Features | 🔄 In Progress |

## 📦 What's Inside

### Hardware Support
- **ESP32-C6 DevKit** - 160MHz RISC-V, 320KB RAM, 8MB Flash
- **ILI9341 TFT Display** - 240x320 color display (SPI, optional)
- **SD Card Module** - FAT filesystem logging (SPI, optional)
- **UART Bridge** - Bidirectional serial communication

### Software Features
- **FreeRTOS** - Real-time task management
- **ESP-IDF Framework** - Native ESP32-C6 support
- **DMA Acceleration** - Fast display updates
- **Interrupt-Based Detection** - Accurate timing analysis
- **VFS Integration** - Standard file operations

## 🎓 Learning Resources

This project demonstrates:
- ESP32-C6 peripheral configuration (UART, SPI, GPIO, I2C)
- Interrupt-driven timing analysis
- SPI bus sharing between multiple devices
- FreeRTOS task management
- Hardware abstraction and graceful degradation
- Professional embedded systems architecture

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🤝 Contributing

We welcome contributions! Please check our [GitHub repository](https://github.com/thenisvan/SerialyTTY) for the latest updates and contribution guidelines.

---

**Built with** ❤️ **for embedded systems developers**
