# ❓ FAQ & Troubleshooting Guide

Common questions, issues, and solutions for SmvIT USB-TTL Bridge.

**Table of Contents**
- [Frequently Asked Questions](#frequently-asked-questions)
- [Troubleshooting by Symptom](#troubleshooting-by-symptom)
- [Hardware Issues](#hardware-issues)
- [Software Issues](#software-issues)
- [Performance Optimization](#performance-optimization)
- [Advanced Troubleshooting](#advanced-troubleshooting)

---

## Frequently Asked Questions

### Q1: What's the maximum baud rate SmvIT supports?

**A:** SmvIT officially supports up to **115200 bps**. This is the maximum for reliable auto-detection and bridging on ESP32-C6.

**Can I try higher rates?**
- Yes, manually edit `src/main.cpp` to set a specific baud rate
- However, auto-detection may not work reliably above 115200
- Test thoroughly with your specific hardware

### Q2: Does SmvIT work without a display?

**A:** Yes, absolutely! The display is **optional**.

- Disable in code: Set `tryDisplay = false` in `src/main.cpp`
- All features work via serial terminal
- Display adds convenience but isn't required

### Q3: Can I use SmvIT without an SD card?

**A:** Yes! SD card is also **optional**.

- Device works perfectly with just the ESP32-C6
- Logging simply doesn't occur without SD card
- Graceful degradation: device continues normally

### Q4: What target devices work with SmvIT?

**A:** Any device with a standard UART interface:

- ✓ Arduino boards
- ✓ Microcontroller development kits
- ✓ Embedded systems
- ✓ IoT devices
- ✓ GPS modules
- ✓ Sensors with serial output
- ✓ Industrial equipment

**Requirements:**
- Standard UART (TTL or RS-232 with level shifter)
- 3.3V or 5V logic levels (use level shifter if needed)
- Baud rate between 9600-115200 bps

### Q5: How do I power SmvIT?

**A:** Multiple options:

1. **USB-C from computer** - Simplest, ~500 mA available
2. **USB-C from wall adapter** - More stable power, can deliver >1A
3. **USB power bank** - Portable option
4. **External 5V supply** - Connect to VUSB pin if needed

**Power requirements:**
- Typical: 100-200 mA
- Display adds ~50 mA
- SD card adds ~50 mA
- Total: ~250-300 mA

### Q6: Can SmvIT detect a baud rate automatically?

**A:** Yes, that's a core feature!

- Press `D` in menu to start detection
- Takes 2-5 seconds
- Works with 9600, 19200, 38400, 57600, 115200 bps
- Displays confidence level

**Limitations:**
- Target device must be sending data or responding
- Requires proper electrical connection
- May fail if signal is very noisy

### Q7: What happens if baud detection fails?

**A:** If detection fails:

1. Error message displayed: "Detection failed"
2. Returns to menu
3. Options:
   - Try again (press `D`)
   - Manually set baud rate (press `S` for settings)
   - Check connections and power

### Q8: How do I exit bridge mode?

**A:** Type the escape sequence:

```
Type: ~~~
```

**Important:**
- Must be three tildes exactly
- Nothing before or after on the line
- Hit Enter/Return after typing
- Immediately exits bridge mode

**Alternative:**
- Power cycle the device (hard reset)
- Send `Ctrl+C` in some terminals (may not work in all)

### Q9: Can I log binary data?

**A:** Yes! SmvIT automatically logs binary data as hex dumps.

**Example log:**
```
[2025-12-08 14:31:06] [DATA] TX: 48 65 6C 6C 6F 20 57 6F 72 6C 64
```

**Interpretation:**
- `48 65 6C 6C 6F` = "Hello"
- `20` = space
- `57 6F 72 6C 64` = "World"

**Tools for hex conversion:**
- Online: rapidtables.com/convert/number/hex-to-ascii.html
- Linux: `echo "48656C6C6F" | xxd -r -p`
- Python: `bytes.fromhex("48656C6C6F").decode()`

### Q10: How much data can I log?

**A:** Limited by **SD card capacity**.

- Typical 32 GB SD card: ~32 billion bytes
- At 115200 bps: ~30 hours of continuous data
- Practical use: Hundreds of sessions

**To maximize:**
- Use high-capacity SD card (64 GB+)
- Format as FAT32 (not exFAT)
- Monitor free space occasionally

### Q11: Is SmvIT secure?

**A:** SmvIT **does not encrypt** data.

- Suitable for lab/development use
- Not for secure/sensitive applications
- All data logged in plaintext to SD card
- Data accessible via USB serial

**For sensitive use:** Add encryption at application level

### Q12: Can multiple instances of SmvIT work together?

**A:** Technically yes, but **not recommended**.

- Each would need its own target device
- Each would need its own USB port
- Configuration would be complex
- Single SmvIT usually sufficient

**Better solution:** Use multiple ESP32 boards with SmvIT firmware

### Q13: What's the warranty/support?

**A:** SmvIT is **open source, provided as-is**.

- No official warranty
- Community support via GitHub
- Source code available for modification
- MIT license for code

### Q14: Can I commercialize SmvIT?

**A:** Yes, under certain conditions:

- MIT license allows commercial use
- Must include license text
- CC-BY-4.0 for documentation (must attribute)
- See LICENSE files in repository

### Q15: How often should I update firmware?

**A:** Updates available when:

- Bug fixes are released
- New features added
- Security issues discovered

**Recommendation:**
- Check GitHub releases monthly
- Update if critical bugs or security issues
- Stay with stable version for production use

---

## Troubleshooting by Symptom

### Device Not Detected by Computer

**Symptoms:**
- USB device doesn't appear in system
- Serial port not showing up
- "Unknown device" in Device Manager

**Solution Steps:**

1. **Check USB cable**
   - Try different USB-C cable (data cable, not charge-only)
   - Test with different USB port
   - Avoid USB hubs, use direct computer port

2. **Check device power**
   - Look for LED on ESP32 board (usually blue/red)
   - If no LED light, power supply issue
   - Try different power source

3. **Install drivers (Windows only)**
   - Download: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
   - Install for your Windows version
   - Reboot computer

4. **Reset device**
   - Unplug USB cable
   - Wait 5 seconds
   - Reconnect
   - Check Device Manager

5. **Try different computer**
   - Borrow another computer to test
   - If detected on other computer, your driver issue
   - If not detected anywhere, hardware issue

**If still not detected:**
- Device may have hardware failure
- Try flashing original ESP32 firmware
- Contact manufacturer

---

### Serial Monitor Shows Garbage/Corrupted Text

**Symptoms:**
- Random characters displayed
- Text looks like: `>>>⌐║╟║╜╪`
- Output unreadable

**Solution Steps:**

1. **Check baud rate**
   - Set to exactly **115200 bps**
   - Verify in terminal settings
   - Even 115000 or 115400 will cause corruption

2. **Check terminal settings**
   - Data bits: 8
   - Stop bits: 1
   - Parity: None
   - Flow control: None (RTS/CTS off)

3. **Test with different terminal**
   - Try: `screen`, `picocom`, PuTTY, Arduino IDE
   - If one works but another doesn't, terminal issue

4. **Check USB cable**
   - Noisy USB cable can cause corruption
   - Try different USB-C cable
   - Try different USB port
   - Move away from other electronics

5. **Power supply issue**
   - Low voltage causes unstable serial
   - Try different USB power source
   - Use USB 3.0 port (provides more power)
   - Use powered USB hub

**Example correct settings:**
```bash
# Linux/macOS:
screen /dev/ttyUSB0 115200,cs8,-parenb,-cstopb

# Or:
picocom -b 115200 /dev/ttyUSB0
```

---

### Device Boots But Menu Doesn't Appear

**Symptoms:**
- Device shows boot messages but hangs
- No menu after "State: Menu"
- Device is running but unresponsive

**Solution Steps:**

1. **Wait a bit longer**
   - Initialization can take 5-10 seconds
   - Display rendering takes time
   - Wait 15 seconds total

2. **Check for display errors**
   - Look for display-related errors in output
   - If error, display may not be working
   - Edit code: `tryDisplay = false`
   - Rebuild and upload

3. **Check SD card initialization**
   - Look for SD card errors
   - If "SD card not detected", that's OK
   - If error mounting, format SD card
   - Edit code: `trySDCard = false` to test

4. **Hardware issue**
   - If both disabled and still hangs, hardware problem
   - Try reflashing firmware
   - Check for compile errors

5. **Memory issue**
   - Check serial output for "Out of memory"
   - Reduce buffer sizes in `config.h`
   - Disable unused features

---

### Baud Detection Says "Not Connected"

**Symptoms:**
- Detection fails immediately
- Message: "Target device not detected"
- No timing data collected

**Solution Steps:**

1. **Check physical connection**
   ```
   SmvIT RX (GPIO16) ← Target TX
   SmvIT TX (GPIO17) → Target RX
   SmvIT GND ← Target GND (IMPORTANT!)
   ```
   - Verify all three connections
   - Check for loose wires
   - Use proper jumper wires (not phone cables!)

2. **Check target device power**
   - Target device must be powered ON
   - LED should be lit (if present)
   - Try power cycling target device

3. **Verify target is sending data**
   - Target must output data continuously
   - Connect terminal to target directly
   - Confirm data is being sent
   - If nothing, target isn't sending

4. **Check voltage levels**
   - ESP32-C6 uses 3.3V logic
   - If target uses 5V, need level shifter
   - Use voltage divider or dedicated IC
   - Wrong voltage can prevent detection

5. **Try manual baud rate**
   - Press `S` for settings
   - Select option to set baud manually
   - Try common rates: 9600, 115200
   - Then press `B` for bridge
   - If this works, detection algorithm issue

6. **Increase detection time**
   - Detection expects continuous data
   - If target sends only periodically, may fail
   - Modify target code to send continuously
   - Or increase detection timeout in `baud_detector.cpp`

---

### SD Card Not Detected

**Symptoms:**
- "SD card not found" message
- No `/sdcard/` mounted
- Log files not created

**Solution Steps:**

1. **Check SD card hardware**
   - Remove and reinsert SD card (click until it clicks)
   - Check contacts are clean
   - Try different SD card to test

2. **Verify connections**
   ```
   SmvIT         SD Card Module
   GPIO7 (MOSI) → DIN (MOSI)
   GPIO2 (MISO) → DO (MISO)
   GPIO6 (SCLK) → CLK (SCLK)
   GPIO1 (CS)   → CS (must be separate from display CS!)
   GND          → GND
   3.3V         → VCC
   ```
   - Check CS pin is GPIO1 (not shared with display!)
   - Verify power connections
   - Look for loose wires

3. **Format SD card**
   - SD card might have wrong filesystem
   - Format as FAT16 or FAT32
   - On Mac: Disk Utility → Erase as MS-DOS
   - On Windows: File Explorer → Format as FAT32
   - On Linux: `mkfs.vfat /dev/sdX1`

4. **Try different SD card**
   - Borrow a different SD card to test
   - If it works, original card is bad
   - Some old cards have compatibility issues

5. **Check for errors in output**
   - Look for specific error codes
   - Example: `ESP_ERR_INVALID_RESPONSE` = bad SD card
   - Document error for debugging

6. **Test SPI bus**
   - If display works but SD card doesn't
   - Shared SPI bus might have conflict
   - Check GPIO pin conflicts in code
   - Make sure CS pins don't collide

---

### Display Shows Garbage or Doesn't Initialize

**Symptoms:**
- Display screen is white/black
- Garbled graphics
- "Display not detected" message
- Display dims/brightens randomly

**Solution Steps:**

1. **Check display connections**
   - All SPI connections (MOSI, MISO, SCLK)
   - Control pins: DC (GPIO3), RST (GPIO4)
   - CS pin: GPIO10
   - Power and ground
   - Visual inspection for bad solder joints

2. **Verify display is powered**
   - Display should have small LED (red or green)
   - If no LED, power issue
   - Check VCC (3.3V) and GND connections
   - Use multimeter to verify voltage

3. **Check reset pin**
   - GPIO4 (RST) must be connected
   - Reset timing critical for initialization
   - Verify soldering on RST pin

4. **Try different display**
   - Borrow ILI9341 display to test
   - If different display works, original is bad
   - Some displays have manufacturing defects

5. **Enable verbose logging**
   - Edit `src/main.cpp`
   - Change: `bool tryDisplay = false;` → `true;`
   - Rebuild and upload
   - Watch serial output for errors
   - Log specific error codes

6. **Test SPI communication**
   - If SD card works, SPI is working
   - If only display fails, might be display issue
   - Try different SPI frequency in `tft_driver.cpp`

7. **Rotation issue**
   - Display might be in wrong orientation
   - Try different rotation settings
   - Edit `tft_driver.cpp` to adjust

---

### Bridge Mode Hangs/Freezes

**Symptoms:**
- Bridge mode starts but doesn't respond
- Can't send data through
- Screen shows but doesn't update
- Device becomes unresponsive

**Solution Steps:**

1. **Try escape sequence**
   - Type `~~~` (three tildes)
   - Wait 2-3 seconds
   - Should return to menu
   - If nothing, try power cycle

2. **Power cycle device**
   - Unplug USB
   - Wait 10 seconds
   - Reconnect
   - Device restarts

3. **Check target device**
   - Target might have crashed
   - Power cycle target device
   - Reconnect to SmvIT
   - Try bridge again

4. **Reduce baud rate**
   - High baud rates can cause issues
   - Try 57600 or 38400 instead
   - Edit settings or firmware
   - Test with lower rate

5. **Check USB cable**
   - Poor connection can cause freezing
   - Try different cable
   - Try different USB port
   - Don't use USB hub

6. **Check for infinite loop**
   - Look at serial output before hang
   - Check if repeating continuously
   - Issue might be in target device
   - Not SmvIT problem if target sends junk

7. **Monitor memory**
   - Bridge mode might run out of memory
   - Check free heap in logs
   - If it drops to ~30 KB, memory issue
   - May need to reduce buffer sizes

---

### Data Loss During Bridging

**Symptoms:**
- Some bytes received don't appear sent
- Missing characters in communication
- Sporadic data corruption

**Solution Steps:**

1. **Check baud rate**
   - Baud must match exactly
   - Even 1% difference causes corruption
   - Use auto-detection to verify

2. **Check cable quality**
   - Bad cable can lose data
   - Try different USB-C cable
   - Use direct connection (no hubs)
   - Keep cable away from noise sources

3. **Reduce baud rate**
   - Data loss higher at 115200
   - Try 57600 or 38400
   - Verify data integrity improves
   - More stable at lower speeds

4. **Check buffer sizes**
   - Buffers might be too small
   - Edit `include/config.h`
   - Increase RX/TX buffer sizes
   - Need ~10-20 KB free RAM

5. **Add delays**
   - Target device might not be fast enough
   - Add small delays between transmissions
   - Example: 10 ms between packets
   - Let SmvIT keep up

6. **Check for flow control**
   - Some devices need RTS/CTS flow control
   - Not currently implemented in SmvIT
   - Might need external hardware
   - Or modify target firmware

---

### High CPU Usage / Device Gets Hot

**Symptoms:**
- Device is very warm to touch
- Battery drains quickly (if on battery)
- CPU usage at 100%

**Solution Steps:**

1. **Normal operation**
   - ESP32-C6 at 160 MHz uses power
   - Bridge mode is CPU-intensive
   - Some heat is normal
   - Becomes warm but not hot

2. **Check for infinite loops**
   - Some code paths might loop excessively
   - Check serial output for repeating messages
   - If stuck in loop, device appears hot
   - May need to exit and restart

3. **Reduce refresh rate**
   - Display refreshes every 50 ms
   - Increase interval to 100 ms or more
   - Edit `display_manager.cpp`
   - Reduces CPU work

4. **Disable display refresh**
   - Display updating uses power
   - `display.update()` is CPU-intensive
   - If display not needed, can disable
   - Call less frequently

5. **Lower clock speed**
   - Not recommended for SmvIT
   - 160 MHz is minimum for stability
   - Lowering might cause issues
   - Leave at default

---

## Hardware Issues

### Level Shifter Requirements

**When needed:**
- Target device uses 5V logic
- ESP32-C6 is 3.3V only
- **Must use level shifter**

**Options:**

1. **Simple voltage divider** (one-way, RX only)
```
Target TX (5V) ──[10K resistor]──┬──→ SmvIT RX (3.3V)
                                  │
                       [20K resistor]
                                  │
                                 GND
```

2. **Dedicated level shifter IC**
- TXS0102: 2-channel, small, cheap
- TXB0104: 4-channel, more flexible
- Available on breakout boards

3. **Arduino-style adapter**
- Some Arduino boards have level shifters
- Can be used as external adapter

### Power Supply Considerations

**For basic operation:**
- USB power from computer: ~500 mA
- Usually sufficient
- Works reliably

**For extended operation:**
- Use USB power adapter (2 A+)
- Provides stable power
- Less noise than computer

**For mobile use:**
- Power bank (10,000 mAh+)
- Several hours operation
- Check mAh rating

**Power calculation:**
```
Device: ~100 mA base
Display: +50 mA (if present)
SD Card: +30 mA (if active)
Total: ~150-180 mA typical
```

---

## Software Issues

### Recompile and Reflash

**When to do this:**
- After code changes
- To fix persistent issues
- To test different settings

**Steps:**

```bash
# Clean build
platformio run -e esp32c6 --target clean

# Full rebuild
platformio run -e esp32c6

# Upload
platformio run -e esp32c6 -t upload

# Monitor
platformio device monitor -b 115200
```

### Restore Factory Firmware

**If you accidentally broke something:**

```bash
# Get latest from repository
git clone https://github.com/thenisvan/SerialyTTY.git
cd SerialyTTY

# Make clean copy
platformio run -e esp32c6 --target erase

# Build and upload
platformio run -e esp32c6 -t upload
```

---

## Performance Optimization

### Throughput Optimization

**Current limits:**
- ~100 KB/s typical for bridge mode
- Limited by USB serial, not ESP32

**To maximize:**
- Use USB 3.0 (faster than 2.0)
- Use direct USB port (not hub)
- Disable display updates during transfers
- Use highest stable baud rate

### Memory Optimization

**Check memory:**
```cpp
printf("Free heap: %d bytes\n", esp_get_free_heap_size());
```

**If low memory (<50 KB):**
- Disable display
- Disable SD logging
- Reduce buffer sizes
- Disable BLE

**Modify buffers in `config.h`:**
```cpp
#define UART_RX_BUFFER_SIZE 1024  // Reduce if needed
#define UART_TX_BUFFER_SIZE 1024  // Reduce if needed
```

---

## Advanced Troubleshooting

### Using JTAG Debugging

**If you have JTAG debugger:**
- Connect JTAG pins to ESP32-C6
- Use ESP-IDF: `idf.py openocd`
- Set breakpoints, step through code

**JTAG pins:**
- JTAG requires additional hardware
- Not typical for basic use
- See ESP32-C6 datasheet

### Analyzing Hex Dumps

**SmvIT logs hex data:**
```
[DATA] TX: 48 65 6C 6C 6F
```

**Convert to ASCII:**
- Online: https://www.rapidtables.com/convert/number/hex-to-ascii.html
- Python: `bytes.fromhex("48656C6C6F").decode()`
- Linux: `echo "48656C6C6F" | xxd -r -p`

### Checking Build Errors

**If compilation fails:**

```bash
# See detailed error
platformio run -e esp32c6 -v

# Check for specific error type
# Look at line numbers reported
# Fix in editor, rebuild
```

**Common errors:**
- `error: 'X' was not declared` - Missing include or variable
- `undefined reference to 'X'` - Linker error, missing implementation
- `error: 'GPIO_NUM_1' undeclared` - Wrong ESP-IDF version

---

## Getting Help

**If you're stuck:**

1. **Check this FAQ first** - Most issues covered
2. **Search issues on GitHub** - Your question might be answered
3. **Read documentation** - USER_GUIDE.md and API_DOCUMENTATION.md
4. **Post issue on GitHub** - Be specific with:
   - Device being used (ESP32-C6 board)
   - What you were doing
   - Error messages (screenshots/logs)
   - Steps to reproduce

---

**Still have questions? Check [GitHub Discussions](https://github.com/thenisvan/SerialyTTY/discussions)! 🤝**
