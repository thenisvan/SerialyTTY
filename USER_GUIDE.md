# 📖 SmvIT User Guide

Complete guide for using the SmvIT USB-TTL Bridge device.

**Table of Contents**
- [Getting Started](#getting-started)
- [Hardware Setup](#hardware-setup)
- [Initial Configuration](#initial-configuration)
- [Operating Modes](#operating-modes)
- [Menu Commands](#menu-commands)
- [Display Interface](#display-interface)
- [Logging and Data Management](#logging-and-data-management)
- [Advanced Usage](#advanced-usage)
- [Tips and Best Practices](#tips-and-best-practices)

---

## Getting Started

### What You'll Need
- ESP32-C6 DevKit with SmvIT firmware
- USB-C cable for power and serial communication
- Target device with UART interface
- Computer with serial terminal software (optional but recommended)

### What's Optional
- ILI9341 TFT Display (240x320)
- MicroSD Card Module
- See [Hardware Setup](#hardware-setup) for connection details

### First Power-Up
1. Connect ESP32-C6 to your computer via USB-C
2. Device automatically boots and initializes all hardware
3. Look for output on serial monitor at 115200 bps
4. Device enters Menu state and awaits input

---

## Hardware Setup

### Step 1: Basic Connections

#### Connect Target Device UART
Connect your target device's UART to the bridge using these GPIO pins:

```
SmvIT (ESP32-C6)    Target Device
─────────────────    ──────────────
GPIO16 (RX)    ──→   TX
GPIO17 (TX)    ←──   RX
GND            ──→   GND
```

**Important:** Ensure common ground connection!

### Step 2: Optional Display Connection

If you have an ILI9341 TFT display, connect the SPI pins:

```
SmvIT          ILI9341 Display
─────────────  ────────────────
GPIO7 (MOSI)   → DIN (MOSI)
GPIO2 (MISO)   → DO (MISO)
GPIO6 (SCLK)   → CLK (SCLK)
GPIO10 (CS)    → CS (Chip Select)
GPIO3 (DC)     → RS (Data/Command)
GPIO4 (RST)    → RESET
GPIO5 (BL)     → Backlight (PWM)
GND            → GND
VCC (3.3V)     → VCC
```

### Step 3: Optional SD Card Connection

To add data logging capability, connect the SD card module:

```
SmvIT          SD Card Module
─────────────  ────────────────
GPIO7 (MOSI)   → DIN (Shared with display)
GPIO2 (MISO)   → DO (Shared with display)
GPIO6 (SCLK)   → CLK (Shared with display)
GPIO1 (CS)     → CS (SD Card Select)
GND            → GND
3.3V           → VCC
```

**Note:** Display and SD card share the SPI bus. Make sure each has a separate CS pin.

### Step 4: Power On
- Connect USB-C to SmvIT
- Device automatically detects connected hardware
- Check serial output for initialization messages
- If display connected, you'll see the boot screen

---

## Initial Configuration

### Enabling the Display in Code

If you just connected a display, enable it in `src/main.cpp`:

```cpp
// Around line 60, change:
bool tryDisplay = false;  // Set to true when display hardware is connected
// To:
bool tryDisplay = true;
```

Then rebuild and upload:
```bash
platformio run -e esp32c6 -t upload
```

### Checking Hardware Detection

The device automatically scans for:
- **Display** - I2C address scan for ILI9341
- **SD Card** - GPIO and SPI initialization
- **UART** - Target device communication

View detected hardware in the Info screen (press `I` in menu).

### Serial Terminal Setup

**Recommended software:**
- **Linux/macOS:** `screen`, `minicom`, or `picocom`
- **Windows:** PuTTY, Tera Term, or Arduino Serial Monitor
- **VS Code:** PlatformIO Serial Monitor

**Settings:**
- Baud rate: 115200
- Data bits: 8
- Stop bits: 1
- Parity: None
- Flow control: None

Example (macOS/Linux):
```bash
screen /dev/ttyUSB0 115200
# Or: picocom -b 115200 /dev/ttyUSB0
```

---

## Operating Modes

SmvIT has multiple operating modes, each serving a specific purpose.

### Mode 1: Menu Mode (Default)
**When:** Device boots, or after exiting bridge mode  
**Purpose:** Navigate features and view information

```
╔════════════════════════════════════╗
║       SmvIT Main Menu              ║
╠════════════════════════════════════╣
║ D: Detect Baud Rate                ║
║ B: Bridge Mode (Auto-detect)       ║
║ S: Settings                        ║
║ I: Info (Hardware Status)          ║
║ H: Help                            ║
║ T: Communication Test              ║
╚════════════════════════════════════╝
```

**How to navigate:**
- Press the letter for your desired command
- Follow on-screen prompts
- Press `X` or `ESC` to return to menu

### Mode 2: Bridge Mode
**When:** Activated by pressing `B` in menu or auto-detected after baud detection  
**Purpose:** Real-time transparent serial communication

**Features:**
- Passes all data through unchanged
- Displays live RX/TX statistics
- Automatically logs to SD card (if available)
- Escape sequence to exit: Type `~~~` to return to menu

**Example Usage:**
```
SmvIT> B
Starting Bridge Mode at detected baud...
[BRIDGE ACTIVE]
RX: 1024 bytes | TX: 512 bytes | Baud: 115200

(Type ~~~ to exit bridge mode)
```

### Mode 3: Baud Detection Mode
**When:** Activated by pressing `D` in menu  
**Purpose:** Automatically discover target device's baud rate

**Process:**
1. Analyzes GPIO timing on RX pin
2. Compares against known baud rates (9600, 19200, 38400, 57600, 115200)
3. Displays detected rate
4. Optionally enters bridge mode at detected rate

**Typical output:**
```
Analyzing baud rate...
[====================] 100%
Detected: 115200 bps ✓
Enter Bridge Mode? (Y/N): _
```

### Mode 4: Testing Mode
**When:** Activated by pressing `T` in menu  
**Purpose:** Verify communication with target device

**Tests performed:**
- TX/RX loopback verification
- Baud rate confirmation
- Data integrity check
- Connection timeout detection

---

## Menu Commands

### D: Detect Baud Rate
Automatically analyzes the target device's baud rate through GPIO timing analysis.

```
Command: D
Status: Analyzing...
Progress: [████████████████░░] 85%
Detected Baud: 115200 bps
Detection Method: GPIO Timing Analysis
Confidence: High ✓
```

**When to use:**
- Unknown target device baud rate
- Verifying communication setup
- Troubleshooting connection issues

### B: Bridge Mode
Enables transparent serial passthrough between host and target device.

```
Command: B
Mode: Bridge Mode - ACTIVE
Baud Rate: 115200 bps
RX Bytes: 2048
TX Bytes: 1536
Log File: /sdcard/logs/log_20251208_143052.txt

>>> Escape sequence: Type '~~~' to exit <<<
```

**While in bridge mode:**
- All received data is immediately forwarded
- Statistics update in real-time
- Data automatically logged (if SD card present)
- Type `~~~` to exit and return to menu

### S: Settings
Access configuration options for the device.

```
Command: S
═══════════════════════════════════
    SETTINGS
═══════════════════════════════════
1. Baud Rate:          115200
2. Log Level:          INFO
3. Auto Bridge:        ON
4. Display Refresh:    50ms
5. Save Settings
6. Load Defaults
═══════════════════════════════════
Select option (1-6): _
```

**Available settings:**
- **Baud Rate:** Default bridge baud rate
- **Log Level:** Verbosity of SD card logging (INFO, DEBUG, DATA)
- **Auto Bridge:** Automatically enter bridge after detection
- **Display Refresh:** Milliseconds between screen updates

### I: Info (Hardware Status)
Display comprehensive device information and hardware status.

```
Command: I
═══════════════════════════════════
    DEVICE INFORMATION
═══════════════════════════════════
Device: SmvIT v1.0
ESP32-C6 DevKit
Firmware: Phase 4 Complete
Build: 20251208

Hardware Status:
  ✓ Display:       Connected (ILI9341)
  ✓ SD Card:       Mounted (/sdcard/logs/)
  ✓ UART Bridge:   Ready
  ✓ Bluetooth:     Stub Ready

Statistics:
  Boot Time:       2.3 seconds
  Total RX:        15,234 bytes
  Total TX:        8,956 bytes
  Current Baud:    115200 bps
  Uptime:          00:04:23

Memory:
  RAM Used:        156 KB / 320 KB
  Free:            164 KB
═══════════════════════════════════
```

### H: Help
Display help information for all commands.

```
Command: H
═══════════════════════════════════
    HELP
═══════════════════════════════════
D  Detect Baud Rate
   Analyzes target device baud rate through GPIO timing.
   Usage: Press D, wait for analysis, view result.

B  Bridge Mode
   Transparent serial passthrough to target device.
   Usage: Press B, type ~~~  to exit.
   Exit Sequence: ~~~

S  Settings
   Configure device parameters and behavior.

I  Info
   Display device status, hardware, and statistics.

T  Test
   Run communication tests with target device.

H  Help
   Show this help message.

X  Exit Menu (return to serial passthrough)
═══════════════════════════════════
```

### T: Test Communication
Perform automated communication tests.

```
Command: T
Running communication tests...

Test 1: UART Connectivity
  Status: ✓ PASS
  Connection stable

Test 2: Baud Rate Verification
  Detected: 115200 bps
  Status: ✓ PASS

Test 3: Data Integrity
  Packets sent: 100
  Packets received: 100
  Errors: 0
  Status: ✓ PASS

Test 4: Response Time
  Avg: 2.3ms
  Max: 4.1ms
  Status: ✓ PASS

Overall Result: ✓ ALL TESTS PASSED
═══════════════════════════════════
```

---

## Display Interface

If connected, the ILI9341 display shows different screens for each mode.

### Boot Screen
Displayed during device initialization.

```
╔════════════════════════════════════╗
║                                    ║
║         ▌▌ SmvIT v1.0 ▌▌          ║
║                                    ║
║      USB-TTL Bridge & Analyzer     ║
║                                    ║
║   Initializing components...       ║
║                                    ║
║   [████████░░░░░░░░░░]  45%        ║
║                                    ║
║   © 2025 SmvIT Development         ║
╚════════════════════════════════════╝
```

### Menu Screen
Shows main command menu.

```
╔════════════════════════════════════╗
║       SmvIT Main Menu              ║
╠════════════════════════════════════╣
║ Hardware Status:                   ║
║  Display: ✓    SD: ✓    BLE: ✓     ║
║                                    ║
║ Commands:                          ║
║  D: Baud Detection                 ║
║  B: Bridge Mode                    ║
║  S: Settings                       ║
║  I: Device Info                    ║
║  H: Help                           ║
║  T: Test Communication             ║
║                                    ║
║ RX: 0 bytes | TX: 0 bytes          ║
╚════════════════════════════════════╝
```

### Bridge Screen
Displayed while in bridge mode with live statistics.

```
╔════════════════════════════════════╗
║    Bridge Mode - ACTIVE            ║
╠════════════════════════════════════╣
║                                    ║
║ Baud: 115200 bps    Status: ON     ║
║                                    ║
║ RX Statistics:                     ║
║  Bytes: 2,048                      ║
║  Rate: 2.1 KB/s                    ║
║                                    ║
║ TX Statistics:                     ║
║  Bytes: 1,536                      ║
║  Rate: 1.8 KB/s                    ║
║                                    ║
║ Uptime: 00:01:23                   ║
║                                    ║
║ Log: /sdcard/logs/log_...txt       ║
║ Escape: Type '~~~' to exit         ║
╚════════════════════════════════════╝
```

### Analysis Screen
Shown during baud rate detection.

```
╔════════════════════════════════════╗
║    Baud Rate Analysis              ║
╠════════════════════════════════════╣
║                                    ║
║ Analyzing GPIO timing...           ║
║                                    ║
║ [████████████████░░░░] 75%         ║
║                                    ║
║ Detected rates (in progress):      ║
║  9600   bps: ░░░░░░░░░░░░░░░░░░    ║
║  19200  bps: ░░░░░░░░░░░░░░░░░░    ║
║  38400  bps: ░░░░░░░░░░░░░░░░░░    ║
║  57600  bps: ░░░░░░░░░░░░░░░░░░    ║
║  115200 bps: ██████████████░░░░░░  ║
║                                    ║
║ Best match: 115200 bps             ║
╚════════════════════════════════════╝
```

---

## Logging and Data Management

### Automatic SD Card Logging

When an SD card is connected, SmvIT automatically logs all activity:

**Log Files:** `/sdcard/logs/log_YYYYMMDD_HHMMSS.txt`

**Log Levels:**
- **INFO** - System events and state changes
- **DATA** - Transmitted and received data (hex dump)
- **BAUD** - Baud rate detection results
- **STATE** - State machine transitions
- **TEST** - Communication test results

**Example Log Content:**
```
[2025-12-08 14:30:52] [INFO] System boot
[2025-12-08 14:30:52] [STATE] State: BOOTING → MENU
[2025-12-08 14:30:55] [INFO] Display initialized (ILI9341)
[2025-12-08 14:30:56] [INFO] SD card mounted (/sdcard/logs/)
[2025-12-08 14:31:02] [BAUD] Baud detection started
[2025-12-08 14:31:04] [BAUD] Detected: 115200 bps (High confidence)
[2025-12-08 14:31:05] [STATE] State: ANALYZING → BRIDGE_MODE
[2025-12-08 14:31:06] [DATA] TX: 48 65 6C 6C 6F 20 57 6F 72 6C 64 (Hello World)
[2025-12-08 14:31:07] [DATA] RX: 4F 4B 0D 0A (OK\r\n)
[2025-12-08 14:32:00] [INFO] Session duration: 00:01:08
```

### Accessing Logs

**From Computer:**
1. Remove microSD card from device
2. Insert into computer's SD card reader
3. Navigate to `/logs/` folder
4. Open `.txt` files with any text editor

**Analyzing Hex Data:**
SmvIT displays binary data as hex dumps. Use online converters:
- `48 65 6C 6C 6F` = "Hello"
- `0D 0A` = `\r\n` (newline)

### Log Rotation

Logs automatically use timestamps to prevent overwriting:
- Each session creates a new file
- Multiple sessions: `log_20251208_143052.txt`, `log_20251208_144530.txt`, etc.
- No manual cleanup needed

---

## Advanced Usage

### Baud Rate Detection Algorithm

SmvIT uses GPIO interrupt timing analysis:

1. **Monitors RX pin** for transitions
2. **Measures bit timing** between edges
3. **Compares patterns** against known baud rates
4. **Returns best match** with confidence level

**Supported rates:** 9600, 19200, 38400, 57600, 115200 bps

**Detection time:** 2-5 seconds typical

### Escape Sequences

**Exit Bridge Mode:**
```
Type: ~~~
(Must be typed standalone, nothing before or after)
```

**Effects:** Immediately returns to menu, preserves statistics

### Data Buffering

SmvIT maintains buffers for smooth communication:

- **RX Buffer:** 1 KB - Prevents data loss during processing
- **TX Buffer:** 1 KB - Ensures outgoing data isn't dropped
- **Auto-flush:** Every 10 log entries to SD card

### Multi-Channel Monitoring

While in bridge mode, simultaneously:
- Routes data between target and host
- Updates display in real-time (20 Hz)
- Writes to SD card every 10 entries
- Counts RX/TX statistics
- Maintains connection timeout detection

---

## Tips and Best Practices

### For Reliable Baud Detection
✓ **DO:**
- Connect target device properly with common ground
- Ensure target device is powered on
- Have target device sending data or responding
- Keep baud rate within 9600-115200 range

✗ **DON'T:**
- Disconnect device while analyzing
- Power off target device during detection
- Use extremely high baud rates (>115200)
- Try to detect without proper power supply

### For SD Card Logging
✓ **DO:**
- Format SD card as FAT16 or FAT32 first
- Use SD card rated for continuous writes
- Check free space occasionally
- Back up important logs

✗ **DON'T:**
- Remove SD card while device is writing
- Use very large SD cards without formatting
- Ignore SD initialization errors
- Rely solely on SD card (always keep computer backup)

### For Display Usage
✓ **DO:**
- Set proper orientation for your viewing angle
- Reduce brightness in dark environments
- Enable display calibration if available
- Clean display gently when dusty

✗ **DON'T:**
- Expose to direct sunlight (fades display)
- Apply pressure to display glass
- Use aggressive cleaning materials
- Block ventilation around display

### For Bridge Mode
✓ **DO:**
- Check statistics periodically
- Use escape sequence `~~~` to exit cleanly
- Monitor data rates for anomalies
- Review logs after sessions

✗ **DON'T:**
- Leave bridge mode running unattended for days
- Ignore connection timeout warnings
- Send random data to test
- Mix different protocols in one session

---

## Troubleshooting Quick Reference

| Problem | Solution |
|---------|----------|
| Device not detected | Check USB cable, try different port |
| No baud detection | Verify target device is connected and powered |
| Display shows garbage | Recheck SPI connections, disable display |
| SD card not found | Format as FAT32, check CS pin, verify power |
| Bridge mode hangs | Use escape sequence `~~~`, power cycle |
| Slow data transfer | Check USB cable quality, reduce log verbosity |
| Memory errors | Check for competing tasks, restart device |

For detailed troubleshooting, see **FAQ_TROUBLESHOOTING.md**.

---

## Getting Help

- **Documentation:** See main README.md and API_DOCUMENTATION.md
- **Issues:** Report bugs on GitHub Issues
- **Discussions:** Ask questions on GitHub Discussions
- **FAQ:** Check FAQ_TROUBLESHOOTING.md for common problems

---

**Happy Bridging! 🚀**
