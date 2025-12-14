# LVGL Integration Guide for SerialyTTY

## Overview

This guide explains how to use LVGL (Light and Versatile Graphics Library) in your SerialyTTY project for creating advanced graphical user interfaces with real-time charts, graphs, and widgets.

## What's Been Added

### Files Created

1. **`include/lv_conf.h`** - LVGL configuration file
   - Memory settings (64KB buffer)
   - Color depth (RGB565/16-bit)
   - Widget enablement (charts, labels, meters, etc.)
   - FreeRTOS tick integration
   
2. **`include/lvgl_driver.h`** - LVGL driver wrapper class
   - Display initialization
   - Chart creation helpers
   - Label management
   - Tick handling

3. **`src/lvgl_driver.cpp`** - Implementation
   - TFT display integration
   - Flush callback for rendering
   - Widget creation utilities

4. **`src/lvgl_example.cpp`** - Usage examples
   - Complete data monitor UI example
   - Real-time chart updates
   - Status display

### Modified Files

1. **`platformio.ini`** - Added LVGL library dependency
2. **`src/CMakeLists.txt`** - Added LCD driver requirements
3. **`include/tft_driver.h`** - Added LVGL support methods
4. **`src/tft_driver.cpp`** - Implemented pixel push functions

## Quick Start

### Step 1: Build the Project

```bash
pio run -e esp32c6
```

LVGL will be automatically downloaded and compiled.

### Step 2: Basic Integration

Add to your `main.cpp`:

```cpp
#include "lvgl_driver.h"
#include "tft_driver.h"

// Global objects
TFTDriver tft;
LVGLDriver lvgl;

void setup_hardware() {
    // ... existing setup ...
    
    // Initialize TFT
    if (tft.begin()) {
        ESP_LOGI(TAG, "TFT initialized");
        
        // Initialize LVGL
        if (lvgl.begin(&tft)) {
            ESP_LOGI(TAG, "LVGL initialized");
            
            // Create a simple label
            lv_obj_t* label = lvgl.createLabel(nullptr, "SerialyTTY");
            lv_obj_center(label);
        }
    }
}

void main_loop() {
    // ... existing loop ...
    
    // Update LVGL every 10ms
    lvgl.tick();
    
    vTaskDelay(pdMS_TO_TICKS(10));
}
```

### Step 3: Create Real-Time Charts

```cpp
// Create a chart for baud rate monitoring
lv_obj_t* chart = lvgl.createRealtimeChart(nullptr, 280, 120);
lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);

// In your loop, add data points
lvgl.addChartDataPoint(chart, 0, baudRateValue);
```

## Features

### 1. Charts for Real-Time Data

```cpp
// Create chart
lv_obj_t* chart = lvgl.createRealtimeChart(parent, width, height);

// Add data continuously
lvgl.addChartDataPoint(chart, seriesIndex, value);
```

**Use Cases:**
- Baud rate detection progress
- Data throughput visualization
- Signal quality metrics
- Error rate tracking

### 2. Status Labels

```cpp
// Create label
lv_obj_t* label = lvgl.createLabel(parent, "Initial Text");

// Update dynamically
lvgl.updateLabel(label, "New Status");
```

**Use Cases:**
- Connection status
- Byte counters
- Error messages
- Mode indicators

### 3. Advanced Widgets

LVGL provides many built-in widgets (enabled in `lv_conf.h`):

- **Meters**: Circular gauges for analog-style displays
- **Bars**: Progress indicators
- **LEDs**: Status indicators
- **Sliders**: Value adjustment
- **Buttons**: Interactive controls
- **Tables**: Data grids
- **Spinners**: Loading indicators

## Configuration

### Memory Settings

In `lv_conf.h`:

```c
#define LV_MEM_SIZE (64U * 1024U)  // 64 KB - adjust based on your needs
```

### Display Settings

```c
#define LV_DEF_REFR_PERIOD 30  // 33 FPS refresh rate
#define LV_COLOR_DEPTH 16       // RGB565 format
```

### Enabling/Disabling Features

To save flash space, disable unused widgets in `lv_conf.h`:

```c
#define LV_USE_CALENDAR   0  // Disable calendar widget
#define LV_USE_KEYBOARD   0  // Disable keyboard widget
```

## Example: Complete Data Monitor

See `src/lvgl_example.cpp` for a full implementation of:

- Multiple real-time charts
- Status labels
- RX/TX byte counters
- Baud rate visualization
- Data rate graphs

## Performance Tips

1. **Refresh Rate**: Default is 30ms (33 FPS). Adjust `LV_DEF_REFR_PERIOD` if needed.

2. **Memory**: Charts with many points consume memory. Use `LV_CHART_POINT_COUNT_MAX` to limit.

3. **Draw Buffer**: Increase `DRAW_BUF_SIZE` in `lvgl_driver.cpp` for faster rendering:
   ```cpp
   static const uint32_t DRAW_BUF_SIZE = 320 * 20; // 20 lines instead of 10
   ```

4. **Task Priority**: Call `lvgl.tick()` regularly (every 10ms) from main loop.

## Troubleshooting

### Display Not Updating

- Ensure `lvgl.tick()` is called regularly
- Check TFT initialization succeeded
- Verify SPI pins in `tft_driver.h`

### Memory Issues

- Reduce `LV_MEM_SIZE` if heap allocation fails
- Decrease `DRAW_BUF_SIZE`
- Disable unused widgets in `lv_conf.h`

### Compilation Errors

- Clean build: `pio run -t clean`
- Update LVGL: `pio pkg update`
- Check `lv_conf.h` is in include path

## Resources

- **LVGL Documentation**: https://docs.lvgl.io/latest/
- **Widget Gallery**: https://docs.lvgl.io/latest/widgets/index.html
- **Examples**: https://github.com/lvgl/lvgl/tree/master/examples
- **Forum**: https://forum.lvgl.io/

## Next Steps

1. **Integrate with Display Manager**: Replace current text-based display with LVGL widgets
2. **Add Touch Support**: Integrate touch controller if available
3. **Create Custom Themes**: Customize colors and styles
4. **Build Interactive Menu**: Replace terminal menu with touchscreen UI
5. **Add Animations**: Use LVGL animations for smooth transitions

## Example Integration Checklist

- [ ] LVGL library compiles successfully
- [ ] TFT display shows LVGL content
- [ ] Charts update in real-time
- [ ] Labels display dynamic text
- [ ] No memory leaks during operation
- [ ] Smooth refresh rate (>20 FPS)
- [ ] Responsive to user interactions

Happy graphing! 📊
