# SmvIT ESP-IDF Transfer & Continuation Plan (macOS → Linux)

**Date**: December 7, 2025  
**Status**: ESP32-C6 ported to ESP-IDF, git repo initialized, pinned platform version to resolve OpenThread git errors  
**Target Device**: ESP32-C6  
**Framework**: ESP-IDF 5.5.0 (via PlatformIO espressif32@6.9.0)  

---

## Project Status Summary

### ✅ Completed
1. **Display optionality**: Display module is optional; core runs without it via presence detection.
2. **Main ported to ESP-IDF**: 
   - Converted from Arduino `setup()/loop()` to ESP-IDF `app_main()` with FreeRTOS
   - Uses `esp_timer_get_time()` for milliseconds
   - UART comms via `esp_driver_uart`
   - Logging via `ESP_LOG`
3. **Baud detector & comm tester**: Ported to ESP-IDF UART APIs with GPIO state detection.
4. **SD logger**: Stubbed to ESP_LOG (no SD-IDF integration yet).
5. **PlatformIO config**: 
   - `[env:esp32dev]` fallback (Arduino, builds successfully)
   - `[env:esp32c6]` (ESP-IDF, pinned to `espressif32@6.9.0`)
6. **CMakeLists.txt**: Static version set (`SmvIT 0.1.0`); static commit descriptions to bypass git-describe.
7. **sdkconfig.defaults**: Disabled OpenThread, IEEE802154, coexist to reduce git issues.
8. **Git repo**: Initialized with `.gitignore` for PlatformIO/ESP-IDF.

### ⚠️ Known Issues
- **ESP-IDF CMake git-describe errors**: PlatformIO's packaged ESP-IDF invokes git queries for OpenThread/bootloader metadata, fails in non-git-repo paths. 
  - **Mitigation applied**: Pinned platform to `espressif32@6.9.0`; static version in CMake; sdkconfig disables.
  - **Status**: Requires testing on Linux with full ESP-IDF toolchain.

### 🔄 In Progress / Partially Complete
- **Display integration**: Currently stubbed; ready for real TFT driver once ESP-IDF builds successfully.
- **SD card VFS**: Stubbed; ready for FATFS + SD driver integration.

---

## Linux PC Setup Instructions

### 1. Clone Repository
```bash
# Clone or copy the git repo to your Linux PC
git clone <repository-url> ~/projects/SmvIT
cd ~/projects/SmvIT
git branch -a
git log --oneline -5  # Verify commit history
```

### 2. Install ESP-IDF Toolchain (Recommended: Native IDF)

#### Option A: Use PlatformIO (Simpler, already configured)
```bash
# Install PlatformIO CLI
pip install platformio

# Install ESP-IDF framework and toolchains (auto via PlatformIO)
platformio run -e esp32c6 --target=clean
platformio run -e esp32c6  # Will download framework on first build

# Fallback Arduino ESP32 build (if esp32c6 fails)
platformio run -e esp32dev
```

#### Option B: Use Native ESP-IDF (More control, if PlatformIO issues persist)
```bash
# Install prerequisites
sudo apt-get update
sudo apt-get install -y build-essential git wget curl cmake ninja-build python3-pip python3-venv

# Clone ESP-IDF repository
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
git checkout v5.5.0  # Match PlatformIO's version

# Install Python dependencies
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt

# Install tools
./install.sh esp32c6

# Source environment in every terminal session:
# source ~/esp/esp-idf/export.sh
```

### 3. Prepare Project for ESP-IDF Build

#### If using native ESP-IDF, create a wrapper CMakeLists.txt:
```bash
# Copy current PlatformIO project to ESP-IDF structure
cp -r ~/projects/SmvIT ~/projects/SmvIT-espidf
cd ~/projects/SmvIT-espidf

# Ensure SDKConfig for ESP-IDF (already present)
cat sdkconfig.defaults

# Build with ESP-IDF CLI
source ~/esp/esp-idf/export.sh
idf.py set-target esp32c6
idf.py menuconfig  # Optional: review config
idf.py build
idf.py flash monitor  # Requires USB connection
```

---

## Build & Deploy Workflow

### Using PlatformIO (Recommended for simplicity)

#### Build for ESP32-C6 (ESP-IDF)
```bash
cd ~/projects/SmvIT
platformio run -e esp32c6
```

**Expected output**:
- CMake configures with app version "SmvIT 0.1.0"
- Compiles `main.cpp`, baud detector, comm tester, display/SD stubs
- Produces `firmware.bin` in `.pio/build/esp32c6/`

#### Build fallback Arduino (ESP32 generic)
```bash
platformio run -e esp32dev
```

#### Upload & Monitor
```bash
# ESP32-C6 (ESP-IDF)
platformio run --target upload --target monitor -e esp32c6

# Or separate commands:
platformio run --target upload -e esp32c6
platformio device monitor --port /dev/ttyUSB0 --baud 115200
```

#### Troubleshooting Build Failures

**Issue**: `fatal: Needed a single revision` (git-describe error)
- **Fix 1**: Ensure `.git` folder exists in repo root:
  ```bash
  ls -la .git/
  git log --oneline -1
  ```
- **Fix 2**: Clean PlatformIO cache and rebuild:
  ```bash
  rm -rf .pio
  platformio run -e esp32c6
  ```
- **Fix 3**: Pin even older platform if `6.9.0` fails:
  ```bash
  # Edit platformio.ini: platform = espressif32@6.8.0
  platformio run -e esp32c6
  ```

---

## Project File Structure

```
SmvIT/
├── README.md                    # Project overview (SK)
├── LINUX_SETUP.md              # This file
├── platformio.ini              # PlatformIO config (esp32dev, esp32c6)
├── CMakeLists.txt              # ESP-IDF project def + static version
├── sdkconfig.defaults          # IDF config (OpenThread disabled)
├── extra_script.py             # PlatformIO hook (static version injection)
├── .gitignore                  # Git ignores for .pio/, build/
├── .git/                       # Git repo
├── include/
│   ├── config.h                # Pin definitions & global config
│   ├── display_manager.h       # Display interface (stubbed for ESP-IDF)
│   ├── display_manager_espidf.h # ESP-IDF display stub
│   ├── baud_detector_espidf.h  # Baud detection (ESP-IDF)
│   ├── comm_tester_espidf.h    # Comm testing (ESP-IDF)
│   ├── sd_logger_espidf.h      # SD logging (stubbed)
│   └── sd_logger.h             # SD logger interface
├── src/
│   ├── main.cpp                # ESP-IDF app_main + FreeRTOS loop
│   ├── display_manager.cpp     # Arduino display (fallback)
│   ├── display_manager_espidf.cpp # ESP-IDF display stub
│   ├── baud_detector_espidf.cpp   # Baud detection impl
│   ├── comm_tester_espidf.cpp     # Comm tester impl
│   ├── sd_logger.cpp           # Arduino SD logger (fallback)
│   ├── sd_logger_espidf.cpp    # ESP-IDF SD logger stub
│   └── comm_tester.cpp         # Arduino comm tester (fallback)
└── .pio/
    └── build/esp32c6/          # Build artifacts (auto-generated)
```

---

## Next Steps on Linux PC

### Phase 1: Validate Build (Week 1)
- [ ] Clone repo and run `platformio run -e esp32c6`
- [ ] If build succeeds: flash to ESP32-C6, verify UART/baud detection over USB console
- [ ] If build fails: apply pinning or switch to native ESP-IDF (Option B above)

### Phase 2: Display Integration (Week 2)
- [ ] Evaluate real TFT libraries for ESP-IDF (e.g., `lvgl`, `esp_lcd`)
- [ ] Replace display stubs in `display_manager_espidf.cpp` with actual driver
- [ ] Test on real ILI9341 hardware via SPI

### Phase 3: SD Card Logging (Week 2–3)
- [ ] Implement FATFS + SD driver in `sd_logger_espidf.cpp`
- [ ] Write test log entries during baud detection/comm testing
- [ ] Verify SD card contains expected data

### Phase 4: Firmware Release (Week 3–4)
- [ ] Integration test: full workflow (detect baud → test comm → log to SD → display output)
- [ ] Build release firmware (`platformio run -e esp32c6`)
- [ ] Document deployment steps

---

## Important Build Flags & Config

### platformio.ini Flags for esp32c6
```ini
build_flags =
  -DCONFIG_LOG_DEFAULT_LEVEL_DEBUG=1   # Enable debug logs
extra_scripts = extra_script.py          # Static version injection
monitor_filters = esp32_exception_decoder # Decode crashes
```

### sdkconfig.defaults (Disables problematic components)
```
CONFIG_OPENTHREAD_ENABLED=n
CONFIG_IEEE802154_ENABLED=n
CONFIG_ESP_COEX_ENABLED=n
```

### ESP-IDF Menuconfig (if using native IDF)
```bash
idf.py menuconfig
# Navigate to:
# - Serial flasher config → COM port, baud rate
# - Component config → OpenThread → disabled (should already be)
```

---

## GPIO Pin Mapping (Config)

From `include/config.h`:
```
ESP32-C6 Pin   Function
4              RX (input from target device)
5              TX (output to target device)
8              TFT DC (Data/Command)
9              SD Card CS
10             TFT CS
11             TFT MOSI
12             TFT SCK
13             TFT MISO
```

---

## Expected Console Output (Successful Build)

```
Processing esp32c6 (platform: espressif32@6.9.0; board: esp32-c6-devkitc-1; framework: espidf)
...
-- App "SmvIT" version: 0.1.0
-- Adding linker script /path/to/sections.ld.in
...
[100%] Built target SmvIT.elf
Generated /Users/*/SmvIT/.pio/build/esp32c6/firmware.bin
================================================ [SUCCESS] Took X.XX seconds ============
```

---

## Running on Device

### Wiring (USB-TTL Converter)
- ESP32-C6 **GND** → USB-TTL **GND**
- ESP32-C6 **TX (GPIO5)** → USB-TTL **RX**
- ESP32-C6 **RX (GPIO4)** → USB-TTL **TX**

### Flash & Monitor
```bash
platformio run --target upload --target monitor -e esp32c6
```

### Expected Runtime Behavior
1. Boot message: `App 'SmvIT' version: 0.1.0`
2. State machine starts in `DETECTING_BAUD`
3. Scans common baud rates (9600, 19200, ..., 115200)
4. On detection: switches to `TESTING_COMM`
5. Sends test message, waits for response
6. If response: logs to SD (stubbed), updates display (stubbed)
7. Runs continuously, monitoring for new data

---

## Git Workflow

### Track your work
```bash
git add <files>
git commit -m "Descriptive message"
git push origin main
```

### Useful commands
```bash
git log --oneline               # View commits
git diff <file>                 # See uncommitted changes
git status                      # Current state
git branch -a                   # View all branches
```

---

## Reference Links

- **ESP-IDF Docs**: https://docs.espressif.com/projects/esp-idf/en/v5.5/
- **ESP32-C6 Technical Reference**: https://www.espressif.com/products/socs/esp32-c6/resources
- **PlatformIO ESP32**: https://docs.platformio.org/en/latest/platforms/espressif32.html
- **ILI9341 TFT Library (Arduino)**: https://github.com/adafruit/Adafruit_ILI9341
- **ESP_LCD (ESP-IDF native)**: https://components.espressif.com/components/esp-idf/esp_lcd/

---

## Contact & Support

If build fails on Linux:
1. Run `platformio run -v -e esp32c6` (verbose mode) and share output
2. Check available USB devices: `ls /dev/ttyUSB* /dev/ttyACM*`
3. If git errors persist, try native ESP-IDF Option B

**Last Updated**: December 7, 2025
