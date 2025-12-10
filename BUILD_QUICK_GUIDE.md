# Quick Build Guide

## Building the Project

### Standard Build
```bash
# Create Git reference workaround files first
mkdir -p .pio/build/esp32c6/CMakeFiles/git-data .pio/build/esp32c6/bootloader/CMakeFiles/git-data
echo "ref: refs/heads/main" > .pio/build/esp32c6/CMakeFiles/git-data/head-ref
echo "ref: refs/heads/main" > .pio/build/esp32c6/bootloader/CMakeFiles/git-data/head-ref

# Build
platformio run -e esp32c6
```

### Clean Build
```bash
# Remove build directory
rm -rf .pio/build/esp32c6

# Create Git references
mkdir -p .pio/build/esp32c6/CMakeFiles/git-data .pio/build/esp32c6/bootloader/CMakeFiles/git-data
echo "ref: refs/heads/main" > .pio/build/esp32c6/CMakeFiles/git-data/head-ref
echo "ref: refs/heads/main" > .pio/build/esp32c6/bootloader/CMakeFiles/git-data/head-ref

# Build
platformio run -e esp32c6
```

### Upload to Device
```bash
platformio run -e esp32c6 -t upload
```

### Monitor Serial Output
```bash
platformio device monitor -e esp32c6
```

### Build & Upload & Monitor
```bash
platformio run -e esp32c6 -t upload && platformio device monitor -e esp32c6
```

## Troubleshooting

### Git Reference Error
If you see:
```
CMake Error: file failed to open for reading (No such file or directory):
  /Users/.../CMakeFiles/git-data/head-ref
```

**Solution**: Run the mkdir/echo commands above before building.

### Missing FreeRTOS Symbols
If you see errors like `vTaskDelay was not declared`:
- Check that `freertos/FreeRTOS.h` and `freertos/task.h` are included
- Verify `#include "freertos/FreeRTOS.h"` comes before `#include "freertos/task.h"`

### Switch Statement Warnings
If you see `-Werror=switch` errors:
- Ensure old backup files (*.cpp.old, *_old.cpp) are removed from src/
- CMakeLists.txt uses GLOB_RECURSE and will compile all .cpp files

## Build Statistics
- **Build Time**: ~40 seconds (clean), ~5 seconds (incremental)
- **RAM Usage**: 3.7% (12 KB / 320 KB)
- **Flash Usage**: 27.1% (283 KB / 1 MB)

## Build Environment
- **Platform**: PlatformIO with ESP-IDF 5.3.1
- **Toolchain**: RISC-V GCC 13.2.0
- **Framework**: ESP-IDF (Espressif IoT Development Framework)
- **Board**: ESP32-C6-DevKitC-1
