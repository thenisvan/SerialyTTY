# Settings System Implementation Plan

## Overview
Implement a comprehensive, hierarchical settings menu system for SerialyTTY with persistent storage, covering all configurable aspects of the device including Bluetooth, hardware, logging, and more.

---

## Current State Analysis

### Existing Settings (Limited)
- Basic baud rate selection (9600-115200)
- Logging toggle (mentioned but not implemented)
- Escape sequence reference
- No Bluetooth configuration
- No persistent storage
- No hardware configuration options

### Missing Critical Features
1. **Bluetooth Settings** - Not accessible in menu
2. **Persistent Configuration** - No NVS/flash storage
3. **Hardware Pin Configuration** - Hard-coded in config.h
4. **Display Settings** - No brightness, timeout, theme options
5. **SD Card Settings** - No log rotation, format options
6. **Advanced UART Settings** - No parity, stop bits, flow control
7. **System Settings** - No device name, timezone, updates

---

## Architecture Design

### 1. Configuration Storage Layer

#### NVS (Non-Volatile Storage) Structure
```cpp
namespace: "smvit_config"

Keys:
- "device_name"        (string) - Device identifier
- "baud_rate"          (u32)    - Default baud rate
- "auto_detect"        (u8)     - Auto-detect enabled
- "ble_enabled"        (u8)     - Bluetooth enabled
- "ble_name"           (string) - BLE device name
- "ble_pin"            (string) - BLE PIN (if using pairing)
- "ble_advertise"      (u8)     - Auto-advertise on boot
- "log_enabled"        (u8)     - SD logging enabled
- "log_level"          (u8)     - Log verbosity (0-5)
- "log_max_size"       (u32)    - Max log file size (KB)
- "display_enabled"    (u8)     - Display on/off
- "display_brightness" (u8)     - Brightness (0-255)
- "display_timeout"    (u16)    - Screen timeout (seconds)
- "display_rotation"   (u8)     - Screen rotation (0-3)
- "uart_parity"        (u8)     - Parity setting
- "uart_stop_bits"     (u8)     - Stop bits (1-2)
- "uart_flow_control"  (u8)     - Flow control type
- "escape_seq"         (string) - Escape sequence
- "timezone_offset"    (i16)    - Timezone offset (minutes)
```

#### Configuration Manager Class
```cpp
class ConfigManager {
public:
    static ConfigManager& getInstance();
    
    bool begin();  // Initialize NVS
    bool save();   // Save all settings to NVS
    bool load();   // Load all settings from NVS
    bool reset();  // Reset to factory defaults
    
    // Getters/Setters for all settings
    // Each setting has validation
    
    // Event callback when settings change
    typedef void (*OnSettingsChanged)();
    void setOnChanged(OnSettingsChanged callback);
    
private:
    ConfigManager();
    struct Settings {
        // All settings as struct members
    } settings;
};
```

---

### 2. Menu System Redesign

#### Hierarchical Menu Structure
```
Main Menu
├── [B] Bridge Mode
├── [D] Detect Baud Rate
├── [S] Settings ──────────────┐
│   ├── [1] UART Settings      │
│   │   ├── Baud Rate          │
│   │   ├── Auto-detect        │
│   │   ├── Parity             │
│   │   ├── Stop Bits          │
│   │   └── Flow Control       │
│   ├── [2] Bluetooth Settings │ ← NEW!
│   │   ├── Enable/Disable     │
│   │   ├── Device Name        │
│   │   ├── Auto-advertise     │
│   │   ├── PIN Code           │
│   │   └── Connection Status  │
│   ├── [3] Display Settings   │ ← NEW!
│   │   ├── Enable/Disable     │
│   │   ├── Brightness         │
│   │   ├── Timeout            │
│   │   └── Rotation           │
│   ├── [4] Logging Settings   │ ← EXPANDED
│   │   ├── Enable/Disable     │
│   │   ├── Log Level          │
│   │   ├── Max File Size      │
│   │   └── View/Clear Logs    │
│   ├── [5] System Settings    │ ← NEW!
│   │   ├── Device Name        │
│   │   ├── Timezone           │
│   │   ├── Date/Time          │
│   │   └── Factory Reset      │
│   └── [6] Advanced           │ ← NEW!
│       ├── Escape Sequence    │
│       ├── Debug Mode         │
│       └── Firmware Info      │
├── [I] Statistics
├── [H] Hardware Info
├── [?] Help
└── [R] Reset
```

#### Enhanced Menu Classes
```cpp
enum MenuScreen {
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_SETTINGS_UART,
    MENU_SETTINGS_BLUETOOTH,
    MENU_SETTINGS_DISPLAY,
    MENU_SETTINGS_LOGGING,
    MENU_SETTINGS_SYSTEM,
    MENU_SETTINGS_ADVANCED,
    MENU_STATS,
    MENU_HARDWARE,
    MENU_HELP
};

class MenuItem {
public:
    const char* label;
    char key;
    void (*handler)();
    MenuScreen nextScreen;
    bool enabled;
};

class MenuPage {
public:
    const char* title;
    std::vector<MenuItem> items;
    MenuScreen parentScreen;
    
    void render();
    void handleInput(char c);
};
```

---

### 3. Bluetooth Settings Implementation

#### Bluetooth Settings Structure
```cpp
struct BluetoothSettings {
    bool enabled;
    char deviceName[32];
    bool autoAdvertise;
    bool requirePairing;
    char pinCode[7];  // 6 digits + null
    uint16_t advertisingInterval;  // ms
    uint8_t txPower;  // dBm
};
```

#### Settings Menu Flow
```
Bluetooth Settings
┌─────────────────────────────────────────┐
│ Bluetooth Configuration                 │
├─────────────────────────────────────────┤
│                                         │
│ Status: [✓] Enabled / [✗] Disabled     │
│                                         │
│ [E] Enable/Disable Bluetooth            │
│ [N] Device Name: SmvIT-Bridge           │
│ [A] Auto-Advertise: [✓] ON / [ ] OFF   │
│ [P] Pairing: [✓] Required / [ ] Open   │
│ [C] PIN Code: ******                    │
│ [I] Advertising Interval: 100ms         │
│ [T] TX Power: 0 dBm                     │
│                                         │
│ Connection Info:                        │
│   Status: Connected to "iPhone"         │
│   MAC: AA:BB:CC:DD:EE:FF               │
│   RSSI: -45 dBm                        │
│                                         │
│ [D] Disconnect Current Device           │
│ [S] Save Settings                       │
│ [M] Back to Settings Menu               │
└─────────────────────────────────────────┘
```

---

## Implementation Phases

### Phase 1: Configuration Infrastructure (Week 1)
**Priority: HIGH**

Files to create/modify:
- `include/config_manager.h` - NEW
- `src/config_manager.cpp` - NEW
- `include/config.h` - Modify to add configurable structs

Tasks:
1. Implement NVS wrapper class
2. Create Settings structure
3. Add validation for all settings
4. Implement save/load/reset methods
5. Add unit tests for config manager

**Acceptance Criteria:**
- Settings persist across reboots
- Invalid settings are rejected
- Factory reset works correctly

---

### Phase 2: Menu System Refactoring (Week 1-2)
**Priority: HIGH**

Files to modify:
- `include/menu_system.h` - Expand significantly
- `src/menu_system.cpp` - Complete rewrite

Tasks:
1. Implement hierarchical menu structure
2. Create MenuItem and MenuPage classes
3. Add navigation history (breadcrumbs)
4. Implement settings editors (numeric, string, bool)
5. Add input validation and formatting

**Acceptance Criteria:**
- Can navigate through all menu levels
- Settings can be edited inline
- Changes are saved properly
- Menu state persists during operations

---

### Phase 3: Bluetooth Settings (Week 2)
**Priority: HIGH - Critical Missing Feature**

Files to modify:
- `include/bluetooth_manager.h` - Add configuration methods
- `src/bluetooth_manager.cpp` - Implement settings
- `src/menu_system.cpp` - Add BLE settings menu

Tasks:
1. Implement BluetoothSettings structure
2. Add methods to apply settings to BLE stack
3. Create Bluetooth settings submenu
4. Add real-time connection status
5. Implement pairing and security features

**Acceptance Criteria:**
- BLE can be enabled/disabled from menu
- Device name is configurable
- Pairing settings work correctly
- Connection status displays in menu

---

### Phase 4: Extended Settings Menus (Week 3)
**Priority: MEDIUM**

Tasks:
1. **Display Settings Menu**
   - Brightness control
   - Timeout configuration
   - Rotation settings
   - Theme selection (future)

2. **Logging Settings Menu**
   - Log level selection
   - File size limits
   - Log viewer
   - Clear logs function

3. **System Settings Menu**
   - Device name editor
   - Timezone configuration
   - RTC time setting
   - About/version info

4. **Advanced Settings Menu**
   - Escape sequence editor
   - Debug mode toggle
   - Performance monitoring

**Acceptance Criteria:**
- All settings accessible and functional
- Changes take effect immediately
- Settings persist correctly

---

### Phase 5: UART Advanced Settings (Week 3-4)
**Priority: MEDIUM**

Tasks:
1. Add parity configuration (None/Even/Odd)
2. Add stop bits selection (1/1.5/2)
3. Add flow control (None/RTS-CTS/XON-XOFF)
4. Update UART initialization to use settings
5. Add validation for setting combinations

**Acceptance Criteria:**
- All UART parameters configurable
- Invalid combinations prevented
- Changes applied without reset

---

### Phase 6: Settings Import/Export (Week 4)
**Priority: LOW**

Tasks:
1. Implement JSON serialization
2. Add export to SD card
3. Add import from SD card
4. Add settings backup/restore
5. Add configuration profiles

**Acceptance Criteria:**
- Settings can be saved to JSON file
- Settings can be loaded from file
- Multiple profiles supported

---

## File Structure Changes

### New Files
```
include/
  config_manager.h          # Configuration management
  settings_editor.h         # Interactive settings editors
  menu_builder.h            # Menu construction utilities

src/
  config_manager.cpp        # NVS storage implementation
  settings_editor.cpp       # Input handlers for settings
  menu_builder.cpp          # Menu page generator
```

### Modified Files
```
include/
  menu_system.h             # Expand significantly
  config.h                  # Add Settings structures
  bluetooth_manager.h       # Add configuration APIs
  display_manager.h         # Add settings support

src/
  menu_system.cpp           # Complete rewrite
  bluetooth_manager.cpp     # Add settings application
  main.cpp                  # Initialize config manager
```

---

## Testing Strategy

### Unit Tests
- ConfigManager save/load/reset
- Settings validation
- Menu navigation logic

### Integration Tests
- Settings persistence across reboots
- BLE configuration changes
- UART parameter changes
- Display settings application

### Manual Tests
- Full menu navigation
- Each setting modification
- Factory reset
- Settings import/export

---

## Success Metrics

1. **Functionality**
   - ✅ All device features configurable
   - ✅ Settings persist correctly
   - ✅ No hard-coded configurations remain
   - ✅ Bluetooth fully configurable

2. **Usability**
   - ✅ Intuitive menu navigation
   - ✅ Clear setting descriptions
   - ✅ Input validation prevents errors
   - ✅ Changes take effect immediately

3. **Reliability**
   - ✅ Settings survive power loss
   - ✅ Factory reset always works
   - ✅ No corrupted settings possible
   - ✅ Backwards compatible with updates

---

## Next Steps

1. **Immediate Actions:**
   - Review and approve this plan
   - Set up development branch
   - Create skeleton files for new classes
   - Begin Phase 1 implementation

2. **Dependencies:**
   - ESP-IDF NVS partition configured
   - Sufficient flash space for settings
   - RTC configured for timestamps

3. **Resources Needed:**
   - Development time: ~4 weeks
   - Testing devices with BLE capability
   - SD card for import/export testing

---

## Future Enhancements (Post-MVP)

- Web interface for configuration
- Mobile app for settings management
- Cloud backup of settings
- Settings templates/presets
- Multi-language menu support
- Voice-guided settings (accessibility)
- Remote configuration via BLE

---

**Document Version:** 1.0
**Date:** December 10, 2025
**Status:** Planning - Ready for Review
