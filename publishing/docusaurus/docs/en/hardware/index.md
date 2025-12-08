---
id: index
title: Hardware Guide
sidebar_position: 2
---

# Hardware Guide

Complete hardware configuration and pin mapping for SerialyTTY.

## ESP32-C6 DevKit

### Board Specifications

| Specification | Value |
|--------------|-------|
| **MCU** | ESP32-C6-WROOM-1 |
| **Architecture** | RISC-V 32-bit |
| **CPU Frequency** | 160 MHz |
| **RAM** | 320 KB SRAM |
| **Flash** | 8 MB |
| **WiFi** | 802.11 b/g/n (2.4 GHz) |
| **Bluetooth** | BLE 5.0 |
| **USB** | USB 2.0 Full Speed (native) |
| **Operating Voltage** | 3.3V |

### Power Requirements
- **Input Voltage**: 5V via USB-C
- **Operating Current**: ~100mA (typical), up to 500mA with peripherals
- **Deep Sleep**: &lt;10µA (when implemented)

## Pin Configuration

### UART Bridge (Primary Function)

| Function | GPIO | Description |
|----------|------|-------------|
| **UART1_RX** | GPIO16 | Receives data from target device |
| **UART1_TX** | GPIO17 | Transmits data to target device |
| **UART0_RX** | GPIO20 | USB Serial (console) |
| **UART0_TX** | GPIO21 | USB Serial (console) |

**UART Configuration:**
- Baud Rate: Auto-detected (9600-115200)
- Data Bits: 8
- Parity: None
- Stop Bits: 1
- Flow Control: None

### TFT Display (ILI9341) - Optional

| Function | GPIO | Description |
|----------|------|-------------|
| **SPI_MOSI** | GPIO7 | Master Out Slave In (shared) |
| **SPI_MISO** | GPIO2 | Master In Slave Out (shared) |
| **SPI_SCLK** | GPIO6 | SPI Clock (shared) |
| **TFT_CS** | GPIO10 | Chip Select (Display) |
| **TFT_DC** | GPIO3 | Data/Command |
| **TFT_RST** | GPIO4 | Reset |
| **TFT_BL** | GPIO5 | Backlight Control |

**SPI Configuration:**
- Clock Speed: 40 MHz
- Mode: SPI Mode 0
- Bit Order: MSB First
- DMA: Channel 1 (enabled)

**Display Specifications:**
- Resolution: 240x320 pixels
- Color Depth: RGB565 (16-bit)
- Interface: 4-wire SPI
- Operating Voltage: 3.3V
- Backlight: PWM controlled (future)

### SD Card Module - Optional

| Function | GPIO | Description |
|----------|------|-------------|
| **SPI_MOSI** | GPIO7 | Master Out Slave In (shared) |
| **SPI_MISO** | GPIO2 | Master In Slave Out (shared) |
| **SPI_SCLK** | GPIO6 | SPI Clock (shared) |
| **SD_CS** | GPIO1 | Chip Select (SD Card) |

**SPI Configuration:**
- Clock Speed: 20 MHz (default), up to 40 MHz
- Mode: SPI Mode 0
- Filesystem: FAT16/FAT32
- Voltage: 3.3V (ensure card is 3.3V compatible!)

**SD Card Requirements:**
- Type: microSD or SD (with adapter)
- Format: FAT32 (recommended)
- Capacity: Up to 32GB tested
- Speed Class: Class 4 or higher

### I2C Bus (Reserved)

| Function | GPIO | Description |
|----------|------|-------------|
| **I2C_SDA** | GPIO8 | I2C Data (future expansion) |
| **I2C_SCL** | GPIO9 | I2C Clock (future expansion) |

**I2C Configuration:**
- Speed: 100 kHz (standard mode)
- Address Range: 0x08-0x77
- Pull-ups: 4.7kΩ external required

### Available GPIO Pins

Reserved for future expansion:

| GPIO | Current Use | Notes |
|------|-------------|-------|
| GPIO0 | Free | Boot mode control (avoid external pull-down) |
| GPIO11 | Free | General purpose |
| GPIO12 | Free | General purpose |
| GPIO13 | Free | General purpose |
| GPIO18 | Free | General purpose |
| GPIO19 | Free | General purpose |
| GPIO22 | Free | General purpose |
| GPIO23 | Free | General purpose |

## Wiring Diagrams

### Minimal Configuration (UART Only)

```
ESP32-C6                Target Device
┌─────────┐            ┌─────────┐
│         │            │         │
│  GPIO16 │◄───────────│   TX    │
│  (RX)   │            │         │
│         │            │         │
│  GPIO17 │────────────►   RX    │
│  (TX)   │            │         │
│         │            │         │
│   GND   │────────────│   GND   │
│         │            │         │
└─────────┘            └─────────┘
```

### Full Configuration (with Display and SD)

```
                  ┌─────────────────┐
                  │   ESP32-C6      │
                  │                 │
    ILI9341       │  GPIO7 (MOSI)   │──┬── SD Card MOSI
    MOSI ─────────│  GPIO2 (MISO)   │──┤
    MISO ─────────│  GPIO6 (SCLK)   │──┴── SD Card SCLK/MISO
    SCLK ─────────│  GPIO10 (CS)    │
    CS   ─────────│  GPIO3  (DC)    │      SD Card
    DC   ─────────│  GPIO4  (RST)   │      CS ────── GPIO1
    RST  ─────────│  GPIO5  (BL)    │
    BL   ─────────│                 │
                  │  GPIO16 (RX)    │──── Target TX
                  │  GPIO17 (TX)    │──── Target RX
                  │                 │
                  │   GND           │──── Common Ground
                  │   3.3V          │──── Power peripherals
                  └─────────────────┘
```

## Hardware Setup Instructions

### Step 1: ESP32-C6 Preparation

1. Inspect the board for physical damage
2. Verify all GPIO pins are accessible
3. Connect USB-C cable to computer
4. Confirm device enumeration (check device manager/dmesg)

### Step 2: TFT Display Connection (Optional)

1. **Verify display voltage**: Must be 3.3V compatible
2. **Connect power first**: VCC to 3.3V, GND to GND
3. **Connect SPI bus**: MOSI, MISO, SCLK (shared with SD)
4. **Connect control pins**: CS, DC, RST, BL

**Important:** 
- Use short, twisted-pair wires for SPI signals
- Keep SPI traces under 10cm if possible
- Add 0.1µF decoupling capacitor near display VCC

### Step 3: SD Card Connection (Optional)

1. **Format SD card**: FAT32 using computer
2. **Verify voltage**: Use 3.3V compatible module only
3. **Connect SPI bus**: Share MOSI, MISO, SCLK with display
4. **Connect CS**: GPIO1 (different from display CS)

**Important:**
- Many SD modules have built-in level shifters - check specs
- Ensure separate chip select (CS) pins
- Test SD card reads/writes on computer first

### Step 4: Target Device Connection

1. Identify target UART pins (TX, RX, GND)
2. **Cross-connect**: Target TX → ESP32 RX (GPIO16)
3. **Cross-connect**: Target RX → ESP32 TX (GPIO17)
4. **Common ground**: Connect GND pins

**Warning:** Ensure voltage levels match (3.3V). Use level shifters for 5V devices.

## Signal Quality Considerations

### SPI Bus Sharing

The display and SD card share the SPI bus (MOSI, MISO, SCLK) but have separate chip select (CS) pins:

- **Display CS**: GPIO10
- **SD Card CS**: GPIO1

**Best Practices:**
- Only one device active at a time (enforced in software)
- Use pull-up resistors on CS lines (10kΩ recommended)
- Minimize capacitive loading with short wires

### UART Signal Integrity

For reliable baud detection:
- Use twisted-pair or shielded cable for long runs
- Keep UART traces away from SPI clock
- Add series termination (22-47Ω) for high speeds
- Avoid running UART parallel to power lines

### Power Distribution

- Add bulk capacitor (100µF) near ESP32-C6 power input
- Add ceramic capacitor (0.1µF) near each peripheral
- Keep power traces wide (>20 mil) and short
- Monitor voltage drop - should stay within ±5%

## Testing Hardware Connections

### Visual Inspection Checklist

- [ ] All connections secure and properly seated
- [ ] No short circuits between adjacent pins
- [ ] Polarity correct (VCC, GND)
- [ ] Signal lines match pin configuration

### Power-On Test

1. Connect USB cable (no peripherals)
2. Verify green LED on ESP32-C6 board
3. Check device appears in serial port list
4. Measure 3.3V between VCC and GND pins

### Peripheral Test

1. **Display**: Should show boot screen on power-up (if enabled)
2. **SD Card**: Check for mount success message in serial log
3. **UART**: Loopback test - connect GPIO16 to GPIO17, verify echo

## Troubleshooting Hardware Issues

### Display Issues

| Symptom | Possible Cause | Solution |
|---------|---------------|----------|
| Blank/white screen | Power issue | Check VCC/GND, measure voltage |
| Garbled display | SPI timing | Reduce SPI clock speed |
| No response | CS/DC/RST wrong | Verify pin connections |
| Dim display | Backlight off | Check BL pin (GPIO5) |

### SD Card Issues

| Symptom | Possible Cause | Solution |
|---------|---------------|----------|
| Mount fails | Format incompatible | Reformat as FAT32 |
| Read errors | Bad card | Try different SD card |
| No detection | CS not working | Verify GPIO1 connection |
| Slow performance | Low speed card | Use Class 10 card |

### UART Issues

| Symptom | Possible Cause | Solution |
|---------|---------------|----------|
| No data received | TX/RX swapped | Cross-check connections |
| Corrupted data | Baud mismatch | Run auto-detection |
| Intermittent errors | Signal quality | Shorten cables, add pull-ups |
| No communication | Voltage mismatch | Check 3.3V/5V levels |

## PCB Design Guidelines (Future)

For custom PCB integration:

1. **Layer Stack**: Minimum 2-layer, 4-layer recommended
2. **SPI Routing**: Match length within 10mm, 50Ω impedance
3. **UART Routing**: Differential pairs if possible, 90Ω impedance
4. **Ground Plane**: Solid ground pour on bottom layer
5. **Decoupling**: 0.1µF ceramic caps within 5mm of ICs
6. **Test Points**: Add TP on all critical signals

---

**Next:** Learn about [Features](../features/index.md) and system capabilities.
