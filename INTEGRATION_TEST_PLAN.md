# SmvIT Integration Test Plan - Phase 4

## Overview
This document outlines the comprehensive integration testing plan for the SmvIT USB-TTL Bridge device. All Phase 2 and Phase 3 features have been implemented and need systematic testing.

## Test Environment Setup

### Hardware Requirements
- ESP32-C6 DevKit
- ILI9341 TFT Display (240x320, SPI interface) - OPTIONAL
- SD Card Module (SPI interface) - OPTIONAL
- Target device with UART (for bridge testing)
- USB cable for power and serial communication

### Pin Configuration
**Display (ILI9341):**
- MOSI: GPIO7
- MISO: GPIO2
- SCLK: GPIO6
- CS: GPIO10
- DC: GPIO3
- RST: GPIO4
- BL: GPIO5

**SD Card:**
- MOSI: GPIO7 (shared with display)
- MISO: GPIO2 (shared with display)
- SCLK: GPIO6 (shared with display)
- CS: GPIO10 (Note: may conflict with display CS)

**UART Bridge:**
- RX: GPIO16 (UART1_RXD)
- TX: GPIO17 (UART1_TXD)

## Test Cases

### TC1: Hardware Detection
**Objective:** Verify I2C and GPIO hardware detection works correctly

**Prerequisites:** None

**Test Scenarios:**

#### TC1.1: No Optional Hardware
- Remove display and SD card
- Boot device
- **Expected:** 
  - Serial output shows "Display not detected"
  - Serial output shows "SD card not detected" or similar
  - Menu shows hardware status as absent
  - Device continues to menu state

#### TC1.2: Display Only
- Connect ILI9341 display
- Remove SD card
- Boot device
- **Expected:**
  - Display shows boot screen with "SmvIT v1.0"
  - Display transitions to menu screen
  - Serial output confirms display initialized
  - SD card reported as not present

#### TC1.3: SD Card Only
- Remove display
- Insert SD card
- Boot device
- **Expected:**
  - Serial output shows SD card detected
  - Log file created in /sdcard/logs/
  - Boot events logged to SD
  - Display reported as not present

#### TC1.4: All Hardware Present
- Connect both display and SD card
- Boot device
- **Expected:**
  - Display shows boot screen
  - SD card mounted successfully
  - Both modules reported in menu
  - Initial logs written to SD card

---

### TC2: Menu System
**Objective:** Verify menu navigation and command handling

#### TC2.1: Main Menu Display
- Boot to menu
- **Expected:**
  - Menu header shown with "Main Menu"
  - Options displayed: D, B, S, I, H
  - Hardware status shown
  - RX/TX counters at 0

#### TC2.2: Menu Navigation
- Press 'H' for help
- **Expected:** Help menu displayed with command descriptions
- Press 'I' for info
- **Expected:** Hardware info screen shown
- Press 'S' for settings
- **Expected:** Settings menu with baud rate options
- Press 'M' to return to main menu
- **Expected:** Return to main menu

#### TC2.3: Settings Menu
- Navigate to settings (S)
- Select different baud rates
- **Expected:** 
  - Baud rate options displayed
  - Selection updates configuration
  - Return to main menu works

---

### TC3: Baud Rate Detection
**Objective:** Verify intelligent baud detection using bit timing analysis

#### TC3.1: Standard Baud Rates
- Connect target device at 9600 baud
- From menu, press 'D' to start detection
- Send data from target device
- **Expected:**
  - State changes to ANALYZING
  - GPIO ISR captures edge timings
  - Baud rate calculated as 9600
  - Display shows detected baud
  - Event logged to SD card

- Repeat for: 19200, 38400, 57600, 115200

#### TC3.2: No Signal Detection
- Disconnect target device
- Press 'D' for detection
- Wait for timeout
- **Expected:**
  - "No data detected" message
  - Returns to menu after timeout
  - Timeout logged

#### TC3.3: Invalid Signal
- Connect target with incorrect voltage levels or noise
- Press 'D' for detection
- **Expected:**
  - Detection attempts
  - Either detects incorrect rate or fails
  - Logs failure and returns to menu

---

### TC4: Bridge Mode
**Objective:** Verify transparent data passthrough and escape sequence

#### TC4.1: Enter Bridge Mode
**Scenario A: Manual Entry**
- From menu, press 'B'
- **Expected:**
  - Message: "=== BRIDGE MODE ==="
  - Instructions shown: "Type ~~~ to exit"
  - State changes to BRIDGE_MODE
  - If display present: shows bridge screen

**Scenario B: After Baud Detection**
- Detect baud rate first
- Automatically enter bridge mode
- **Expected:** Same as Scenario A

#### TC4.2: Data Forwarding USB→Target
- Enter bridge mode
- Type characters in serial terminal
- **Expected:**
  - Characters sent to target UART (TX)
  - TX counter increments
  - If display present: TX bytes updated
  - If SD card present: data logged with "TX" direction

#### TC4.3: Data Forwarding Target→USB
- Target device sends data
- **Expected:**
  - Data appears in serial terminal
  - RX counter increments
  - If display present: RX bytes updated
  - If SD card present: data logged with "RX" direction

#### TC4.4: Bidirectional Communication
- Send data both directions simultaneously
- **Expected:**
  - Both RX and TX work concurrently
  - Counters increment correctly
  - No data corruption
  - Logs capture both directions

#### TC4.5: Escape Sequence Detection
- In bridge mode, type "~~~"
- **Expected:**
  - Bridge mode exits
  - Message: "=== EXITED BRIDGE MODE ==="
  - Returns to menu
  - Final statistics shown (RX/TX bytes)
  - Menu displays updated byte counts

#### TC4.6: Escape Timing
- Type "~" wait 2 seconds
- Type "~" wait 2 seconds  
- Type "~"
- **Expected:**
  - Timeout between characters detected
  - Does NOT exit bridge mode
  - Characters forwarded to target

- Type "~~~" quickly
- **Expected:**
  - Exits bridge mode successfully

#### TC4.7: High Throughput
- Send continuous data stream from target (e.g., 1KB/sec)
- **Expected:**
  - All data forwarded to USB
  - No buffer overruns
  - RX counter accurate
  - Display updates periodically (if present)

---

### TC5: Display Integration
**Objective:** Verify TFT display shows correct information
**Prerequisites:** ILI9341 display connected

#### TC5.1: Boot Screen
- Power on device
- **Expected:**
  - Blue header bar with "SmvIT v1.0"
  - "ESP32-C6" in cyan
  - "USB-TTL Bridge" in green
  - "Booting..." in yellow
  - Screen clears after boot

#### TC5.2: Menu Screen
- Boot to menu
- **Expected:**
  - Header: "Main Menu"
  - White text listing commands
  - Status message area (cyan text)
  - Bottom status bar with RX/TX counts

#### TC5.3: Analyzing Screen
- Start baud detection
- **Expected:**
  - Header: "Baud Detection"
  - Yellow text: "Analyzing signal..."
  - Gray text: "Send data from device"
  - Status updates shown

#### TC5.4: Bridge Mode Screen
- Enter bridge mode
- **Expected:**
  - Header: "Bridge Mode"
  - Green text: "Status: ACTIVE"
  - Baud rate shown in cyan
  - RX/TX byte counts updated
  - Yellow text: "Type ~~~ to exit"

#### TC5.5: Display Updates
- In bridge mode, transfer data
- **Expected:**
  - RX/TX counters update in real-time
  - No screen flicker
  - Updates happen smoothly

---

### TC6: SD Card Logging
**Objective:** Verify all events are logged correctly
**Prerequisites:** SD card inserted and formatted (FAT32)

#### TC6.1: Initialization
- Boot with SD card
- **Expected:**
  - Mount message: "SD card mounted successfully"
  - Directory /sdcard/logs created
  - New log file created with timestamp
  - Initial log entry: "SD Logger initialized"

#### TC6.2: Boot Logging
- Check log file after boot
- **Expected:**
  - Timestamp format: [YYYY-MM-DD HH:MM:SS]
  - Entry: "System boot"
  - Entry: "Components initialized"
  - Entry: State change to MENU

#### TC6.3: Baud Detection Logging
- Perform baud detection
- Check log file
- **Expected:**
  - Entry: State change to ANALYZING
  - Entry: "Baud rate detected: XXXXX bps"
  - Entry: State change to BRIDGE_MODE

#### TC6.4: Data Logging
- Transfer data in bridge mode
- Check log file
- **Expected:**
  - TX data entries with hex format
  - RX data entries with hex format
  - Format: [timestamp] [DATA] DATA-TX: XX XX XX ...
  - Large transfers truncated to 64 bytes with "... (NNN total)"

#### TC6.5: Log File Format
- Examine log file structure
- **Expected:**
  - Filename: /sdcard/logs/log_YYYYMMDD_HHMMSS.txt
  - Human-readable text format
  - Each line: [timestamp] [LEVEL] message
  - Levels used: INFO, BAUD, STATE, DATA

#### TC6.6: Auto-Flush
- Transfer data
- Remove SD card immediately after 10th log entry
- Re-insert and check file
- **Expected:**
  - At least first 10 entries preserved
  - File not corrupted

#### TC6.7: Card Removal
- Remove SD card during operation
- **Expected:**
  - Logging fails gracefully
  - Device continues operating
  - Logs fall back to serial output

---

### TC7: State Machine
**Objective:** Verify all state transitions work correctly

#### TC7.1: Boot Sequence
- Power on
- **Expected Sequence:**
  1. STATE_BOOTING
  2. Hardware detection
  3. STATE_MENU
  4. Display updated at each transition

#### TC7.2: Detection Flow
- From menu, start detection
- **Expected Sequence:**
  1. STATE_MENU
  2. STATE_ANALYZING (on 'D' press)
  3. STATE_BRIDGE_MODE (on successful detection)
  4. STATE_MENU (on escape sequence)

#### TC7.3: Manual Bridge Entry
- From menu, press 'B'
- **Expected Sequence:**
  1. STATE_MENU
  2. STATE_BRIDGE_MODE
  3. Baud rate prompt or use default
  4. STATE_MENU (on escape)

#### TC7.4: Error Recovery
- Start detection with no target
- **Expected:**
  - Timeout occurs
  - Returns to STATE_MENU
  - No crash or hang

---

### TC8: Performance Testing
**Objective:** Verify system performs under load

#### TC8.1: Continuous Operation
- Run bridge mode for 1 hour
- **Expected:**
  - No crashes
  - No memory leaks
  - Counters accurate
  - Display responsive (if present)

#### TC8.2: High Baud Rate
- Test at 115200 baud
- Send continuous data stream
- **Expected:**
  - No data loss
  - Correct forwarding
  - Display updates work

#### TC8.3: SD Card Write Speed
- Log large amount of data
- **Expected:**
  - Logging doesn't slow down bridge
  - No dropped data
  - File writes complete

#### TC8.4: Memory Usage
- Monitor heap memory
- **Expected:**
  - No memory leaks over time
  - Heap doesn't grow continuously
  - System remains stable

---

### TC9: Edge Cases
**Objective:** Test unusual scenarios

#### TC9.1: Rapid State Changes
- Quickly press D, then ~~~, then B, then ~~~
- **Expected:**
  - All state changes handled
  - No crashes
  - Display updates correctly

#### TC9.2: Partial Escape Sequence
- Type "~~" then normal data then "~~~"
- **Expected:**
  - First "~~" forwarded as data
  - Second "~~~" triggers exit

#### TC9.3: Display SPI Conflict
- If display and SD share SPI bus
- Transfer data while logging
- **Expected:**
  - SPI arbitration works
  - No corruption
  - Both devices work

#### TC9.4: Long Menu Session
- Navigate through all menus multiple times
- **Expected:**
  - No memory issues
  - Menus display correctly
  - Commands work consistently

---

## Acceptance Criteria

### Minimum Requirements (Must Pass)
- ✅ Menu system navigable via serial
- ✅ Baud detection works for common rates (9600-115200)
- ✅ Bridge mode forwards data bidirectionally
- ✅ Escape sequence exits bridge mode
- ✅ System stable for 1 hour operation
- ✅ No data corruption during transfer

### Optional Hardware (If Present)
- ✅ Display shows boot screen
- ✅ Display updates in bridge mode
- ✅ SD card logs all events
- ✅ Log files readable and complete

### Performance Targets
- ✅ Bridge mode supports up to 115200 baud
- ✅ Latency < 10ms for data forwarding
- ✅ Display updates at least 1Hz
- ✅ SD logging doesn't block bridge mode
- ✅ Memory usage stable over time

---

## Test Execution

### Phase 1: Basic Functionality (No Optional Hardware)
1. Run TC1.1 - Hardware detection without modules
2. Run TC2 - Menu system tests
3. Run TC3 - Baud detection (if target available)
4. Run TC4 - Bridge mode
5. Run TC7 - State machine

### Phase 2: Display Integration
1. Connect display
2. Run TC1.2 - Detection with display
3. Run TC5 - All display tests
4. Repeat TC2, TC4 with display active

### Phase 3: SD Card Integration
1. Remove display, add SD card
2. Run TC1.3 - Detection with SD card
3. Run TC6 - All SD logging tests
4. Verify log file contents

### Phase 4: Full Integration
1. Connect both display and SD card
2. Run TC1.4 - All hardware present
3. Run complete workflow: Detection → Bridge → Logging
4. Run TC8 - Performance tests
5. Run TC9 - Edge cases

---

## Bug Reporting Template

```
Bug ID: BUG-XXX
Title: [Brief description]
Severity: Critical / High / Medium / Low
Test Case: TCXX.X
Hardware Config: [Display: Y/N, SD Card: Y/N]

Steps to Reproduce:
1. 
2. 
3. 

Expected Result:


Actual Result:


Additional Info:
- Serial output: 
- Log file content:
- Memory state:
```

---

## Success Criteria
All test cases in Phase 1 (Basic Functionality) must pass for minimal viable product.
At least 80% of optional hardware tests must pass when hardware is present.
No critical or high severity bugs remaining.

---

## Next Steps After Testing
1. Document any bugs found
2. Fix critical issues
3. Re-test failed cases
4. Update documentation with known limitations
5. Prepare for Phase 5 (Custom Applications)
