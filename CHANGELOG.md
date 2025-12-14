# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Planned
- Protocol analyzers for I2C, SPI, and CAN
- Real-time data visualization charts
- Dedicated mobile application
- Cloud logging and analytics integration

---

## [1.1.0] - 2025-12-14

### Added
- **Bluetooth Low Energy (BLE) Control**
  - Full Nordic UART Service (NUS) implementation
  - Remote command control: S, B, M, D, R, I, H
  - Wireless serial data streaming in bridge mode
  - Auto-advertise configuration with customizable device name
  - nRF Connect app compatibility
  - Runtime BLE settings menu
  - MTU negotiation support (20-500 bytes)
  
- **Bridge Mode Enhancements**
  - Bidirectional data forwarding (USB ↔ UART ↔ BLE)
  - Simultaneous USB and BLE data streaming
  - UART driver error handling improvements
  
- **Configuration System**
  - Interactive Bluetooth configuration menu
  - Device name customization
  - Auto-advertise toggle
  - Settings persistence to NVS (Non-Volatile Storage)
  
- **Documentation**
  - Comprehensive SDLC documentation
  - CI/CD pipeline configuration
  - Mermaid diagrams for architecture and workflows
  - BLE usage scenarios and examples
  - Troubleshooting guide for BLE issues
  
### Changed
- Updated README with BLE features and diagrams
- Enhanced main state machine to process BLE commands
- Improved error logging for UART operations
- Optimized BLE response formats to fit MTU limits

### Fixed
- BLE command processing for single-character commands
- UART read error handling in bridge mode
- Response truncation issues with 20-byte MTU limit
- Memory initialization for BluetoothManager

### Security
- Added Snyk security scanning integration
- Implemented secret detection in CI/CD pipeline
- Enhanced input validation for BLE commands

---

## [1.0.0] - 2025-12-08

### Added
- **Core Features**
  - Intelligent baud rate detection (9600-115200 bps)
  - GPIO interrupt-based bit timing analysis
  - Transparent serial bridging with escape sequence (`~~~`)
  - Automatic hardware detection for optional peripherals
  
- **Display System**
  - LVGL 9.4.0 graphics framework integration
  - ILI9341 TFT display support (240x320)
  - Multi-screen UI: Boot, Menu, Analyzing, Bridge
  - Real-time RX/TX statistics display
  - DMA-accelerated graphics rendering
  
- **Data Logging**
  - SD card support with FAT32 filesystem
  - Timestamped log files (log_YYYYMMDD_HHMMSS.txt)
  - Multiple log levels: INFO, DATA, BAUD, STATE, TEST
  - Hex dump formatting for binary data
  - Automatic flush to prevent data loss
  
- **Interactive Menu System**
  - ANSI-formatted terminal interface
  - Command-line navigation
  - Hardware information display
  - Settings configuration
  - Real-time status monitoring
  
- **Communication Testing**
  - Built-in communication tester
  - Test pattern generation
  - Device verification utilities
  
- **Hardware Support**
  - ESP32-C6 DevKitC-1 platform
  - USB-C serial communication (115200 bps)
  - SPI peripherals (display, SD card)
  - I2C hardware detection
  
### Documentation
- Comprehensive README with feature list
- BUILD_GUIDE.md for detailed build instructions
- USER_GUIDE.md for end-user documentation
- API_DOCUMENTATION.md for developers
- INTEGRATION_TEST_PLAN.md for testing
- FAQ_TROUBLESHOOTING.md for common issues

---

## [0.9.0] - 2025-11-30 (Beta)

### Added
- Initial beta release
- Basic baud detection functionality
- Simple serial bridging
- USB serial interface
- Command-line menu prototype

### Changed
- Refactored state machine architecture
- Improved error handling

### Fixed
- Memory leaks in UART driver
- Display initialization race conditions

---

## [0.5.0] - 2025-11-15 (Alpha)

### Added
- Proof-of-concept implementation
- Basic ESP32-C6 setup
- UART initialization
- Simple echo mode

### Known Issues
- Baud detection unstable
- No peripheral support
- Limited error handling

---

## Release Types

### Version Numbering
- **Major (X.0.0):** Breaking changes, major new features
- **Minor (x.X.0):** New features, backward compatible
- **Patch (x.x.X):** Bug fixes, no new features

### Release Channels
- **Stable (main):** Production-ready releases
- **Beta (release/*):** Feature-complete, testing phase
- **Alpha (dev):** Early development, frequent changes
- **Nightly:** Automated daily builds from dev branch

---

## Upgrade Instructions

### From 1.0.0 to 1.1.0

**Breaking Changes:** None

**New Features:**
- Bluetooth Low Energy control is now available
- Configure via Settings menu (option 2 from main menu)

**Migration Steps:**
1. Upload new firmware via PlatformIO
2. Device will boot normally with BLE disabled by default
3. Enable BLE via Settings → Bluetooth Settings
4. Configure device name if desired
5. Settings are saved automatically to NVS

**Verification:**
```
# Check version
I (xxx) MAIN: SerialyTTY v1.1.0

# Check BLE status
I (xxx) BLE_UART: Bluetooth initialized: <device_name>
I (xxx) BLE_UART: Advertising started
```

### From Beta to 1.0.0

**Breaking Changes:**
- Configuration file format changed
- API signatures updated
- State machine redesigned

**Migration Required:**
- Re-flash firmware (erase flash recommended)
- Reconfigure all settings
- Update any custom scripts

---

## Deprecation Warnings

### Deprecated in 1.1.0
- None

### Planned Deprecations
- Legacy menu command format (to be replaced in 2.0.0)
- Direct GPIO access functions (use HAL in 2.0.0)

---

## Security Advisories

### 1.1.0
- No known vulnerabilities

### 1.0.0
- Initial secure release
- Snyk scan: 0 critical, 0 high, 0 medium issues

---

## Contributors

### Version 1.1.0
- Denis Nisan (@thenisvan) - BLE implementation, documentation

### Version 1.0.0
- Denis Nisan (@thenisvan) - Core development, architecture

---

## Links

- [Releases](https://github.com/thenisvan/SerialyTTY/releases)
- [Issue Tracker](https://github.com/thenisvan/SerialyTTY/issues)
- [Documentation](https://github.com/thenisvan/SerialyTTY)
- [License](./LICENSE)

---

*For detailed commit history, see [GitHub Commits](https://github.com/thenisvan/SerialyTTY/commits/main)*
