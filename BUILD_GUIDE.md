# Build Guide for USB-TTL Sniffer

## Platform Support

### ✅ Supported (macOS ARM64)
- **ESP32 (standard)** - Full PlatformIO support
- Use environment: `esp32dev`

### ⚠️ ESP32-C6 on macOS ARM64
PlatformIO currently **lacks precompiled packages** for ESP32-C6 on macOS ARM64 (Apple Silicon).

**Workarounds for ESP32-C6:**
1. **Use Docker with x64 emulation** (recommended for local development)
2. **Use CI/CD on Linux x64** (recommended for production)
3. **Use a Linux or Windows x64 machine**
4. **Use ESP-IDF framework** (requires code changes)

---

## Quick Start (ESP32 Standard)

### Build
```bash
platformio run -e esp32dev
```

### Upload
```bash
platformio run -e esp32dev -t upload
```

### Monitor Serial
```bash
platformio device monitor -b 115200
```

---

## ESP32-C6 Build (Docker Method)

If you want to build for ESP32-C6 on macOS ARM64, use Docker:

### 1. Install Docker Desktop
Download from: https://www.docker.com/products/docker-desktop

### 2. Build using Docker
```bash
docker run --rm --platform linux/amd64 \
  -v $(pwd):/project -w /project \
  infinitecoding/platformio-core \
  pio run -e esp32c6
```

### 3. Upload (requires physical connection)
After building in Docker, copy the firmware and upload:
```bash
# Copy firmware from Docker build
cp .pio/build/esp32c6/firmware.bin ./firmware_c6.bin

# Upload using esptool (install: pip install esptool)
esptool.py --chip esp32c6 --port /dev/cu.usbserial-* write_flash 0x0 firmware_c6.bin
```

---

## Project Features

### Display Handling (No Bootloop)
The project is designed to **continue running** even when:
- Display is not connected
- Display fails to initialize
- Display hardware is faulty

When display is not detected:
- System logs to Serial: `"Display not detected — continuing without display."`
- All display operations become safe no-ops
- Core functionality (baud detection, SD logging, testing) continues normally

### Pin Configuration
Edit `include/config.h` to match your hardware:

```cpp
// Serial Sniffer Pins
#define RX_PIN 4
#define TX_PIN 5

// Display Pins (TFT)
#define TFT_CS    10
#define TFT_DC    8
#define TFT_RST   -1
#define TFT_MOSI  11
#define TFT_SCK   12
#define TFT_MISO  13

// SD Card
#define SD_CS     9
```

---

## Verification Steps

### 1. Build Verification
```bash
platformio run -e esp32dev
```
Expected: `[SUCCESS]` with compilation warnings (pin redefinition) - these are **safe to ignore**.

### 2. Upload & Test
```bash
platformio run -e esp32dev -t upload && platformio device monitor -b 115200
```

### 3. Expected Serial Output
```
=== USB-TTL SNIFFER STARTING ===
Display not detected — continuing without display.
System boot
Components initialized
Ready
```

### 4. Runtime Behavior
- **With display connected:** Boot screen, status updates, state transitions shown on TFT
- **Without display:** All operations continue via Serial logging only (no bootloop)

---

## Troubleshooting

### Build Fails on macOS ARM64 for ESP32-C6
**Cause:** No precompiled toolchain packages for RISC-V ESP32-C6 on ARM64.

**Solutions:**
1. Use Docker method (see above)
2. Use `esp32dev` environment for development/testing
3. Set up CI/CD on GitHub Actions (Linux x64) for C6 builds

### Display Warnings
Warnings like `"TFT_MISO" redefined` are **expected** and safe. They occur because:
- `config.h` defines pin mappings
- `TFT_eSPI` library has default definitions
- Your definitions take precedence (correct behavior)

### Serial Monitor Shows Garbage
- Check baud rate: `platformio device monitor -b 115200`
- Ensure USB-CDC is enabled (already set in `platformio.ini`)

---

## CI/CD Setup (GitHub Actions)

For automated ESP32-C6 builds, add `.github/workflows/build.yml`:

```yaml
name: PlatformIO CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: actions/cache@v3
        with:
          path: |
            ~/.cache/pip
            ~/.platformio/.cache
          key: ${{ runner.os }}-pio
      - uses: actions/setup-python@v4
        with:
          python-version: '3.11'
      - name: Install PlatformIO
        run: pip install platformio
      - name: Build ESP32-C6
        run: platformio run -e esp32c6
      - name: Build ESP32
        run: platformio run -e esp32dev
```

---

## Next Steps

1. **Test without display:** Unplug display, flash, verify no bootloop via Serial
2. **Test with display:** Connect display, verify boot screen and status updates
3. **Configure pins:** Update `config.h` for your specific ESP32-C6 module pinout
4. **Add TFT_eSPI config:** Create `TFT_eSPI/User_Setup.h` in library folder if needed

---

## Support

- **PlatformIO Docs:** https://docs.platformio.org/
- **ESP32-C6 Datasheet:** https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf
- **TFT_eSPI Config:** https://github.com/Bodmer/TFT_eSPI
