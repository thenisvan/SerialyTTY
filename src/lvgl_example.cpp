/**
 * @file lvgl_example.cpp
 * @brief Example integration of LVGL with SerialyTTY project
 * 
 * This file demonstrates how to:
 * 1. Initialize LVGL with TFT display
 * 2. Create real-time charts for serial data
 * 3. Display status labels
 * 4. Update graphics in the main loop
 */

#include "lvgl_driver.h"
#include "tft_driver.h"
#include "display_manager.h"
#include "esp_log.h"

static const char* TAG = "LVGL_EXAMPLE";

// Example: Create a data monitoring screen with charts
class LVGLDataMonitor {
private:
    LVGLDriver* lvgl;
    lv_obj_t* baudChart;
    lv_obj_t* dataRateChart;
    lv_obj_t* statusLabel;
    lv_obj_t* rxLabel;
    lv_obj_t* txLabel;
    
public:
    LVGLDataMonitor(LVGLDriver* lvglDriver) : lvgl(lvglDriver) {
        baudChart = nullptr;
        dataRateChart = nullptr;
        statusLabel = nullptr;
        rxLabel = nullptr;
        txLabel = nullptr;
    }
    
    void createUI() {
        if (!lvgl) return;
        
        // Get active screen
        lv_obj_t* scr = lv_screen_active();
        
        // Create title label
        lv_obj_t* title = lvgl->createLabel(scr, "SerialyTTY Monitor");
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
        lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
        
        // Create status label
        statusLabel = lvgl->createLabel(scr, "Status: Waiting");
        lv_obj_align(statusLabel, LV_ALIGN_TOP_LEFT, 10, 40);
        
        // Create RX/TX counters
        rxLabel = lvgl->createLabel(scr, "RX: 0 bytes");
        lv_obj_align(rxLabel, LV_ALIGN_TOP_LEFT, 10, 60);
        
        txLabel = lvgl->createLabel(scr, "TX: 0 bytes");
        lv_obj_align(txLabel, LV_ALIGN_TOP_RIGHT, -10, 60);
        
        // Create baud rate chart
        baudChart = lvgl->createRealtimeChart(scr, 280, 80);
        lv_obj_align(baudChart, LV_ALIGN_CENTER, 0, -20);
        
        // Create data rate chart
        dataRateChart = lvgl->createRealtimeChart(scr, 280, 60);
        lv_obj_align(dataRateChart, LV_ALIGN_BOTTOM_MID, 0, -10);
        
        ESP_LOGI(TAG, "LVGL UI created");
    }
    
    void updateStatus(const char* status) {
        if (statusLabel) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Status: %s", status);
            lvgl->updateLabel(statusLabel, buf);
        }
    }
    
    void updateCounters(uint32_t rxBytes, uint32_t txBytes) {
        if (rxLabel) {
            char buf[32];
            snprintf(buf, sizeof(buf), "RX: %lu bytes", (unsigned long)rxBytes);
            lvgl->updateLabel(rxLabel, buf);
        }
        
        if (txLabel) {
            char buf[32];
            snprintf(buf, sizeof(buf), "TX: %lu bytes", (unsigned long)txBytes);
            lvgl->updateLabel(txLabel, buf);
        }
    }
    
    void addBaudRateData(uint32_t baudRate) {
        if (baudChart) {
            // Normalize baud rate to 0-100 scale for visualization
            int32_t value = (baudRate / 1000);  // Scale down
            if (value > 100) value = 100;
            lvgl->addChartDataPoint(baudChart, 0, value);
        }
    }
    
    void addDataRateData(uint32_t bytesPerSecond) {
        if (dataRateChart) {
            // Scale to 0-100
            int32_t value = bytesPerSecond;
            if (value > 100) value = 100;
            lvgl->addChartDataPoint(dataRateChart, 0, value);
        }
    }
};

// Example usage in main.cpp:
/*

// Global objects
TFTDriver tft;
LVGLDriver lvgl;
LVGLDataMonitor* dataMonitor = nullptr;

void setup_lvgl() {
    // Initialize TFT first
    if (!tft.begin()) {
        ESP_LOGE(TAG, "TFT init failed");
        return;
    }
    
    // Initialize LVGL
    if (!lvgl.begin(&tft)) {
        ESP_LOGE(TAG, "LVGL init failed");
        return;
    }
    
    // Create data monitor UI
    dataMonitor = new LVGLDataMonitor(&lvgl);
    dataMonitor->createUI();
    
    ESP_LOGI(TAG, "LVGL setup complete");
}

void loop() {
    // Update LVGL (call this every ~10ms)
    lvgl.tick();
    
    // Update your data
    if (dataMonitor) {
        dataMonitor->updateStatus("Running");
        dataMonitor->updateCounters(rxBytesCount, txBytesCount);
        dataMonitor->addBaudRateData(currentBaudRate);
        dataMonitor->addDataRateData(bytesPerSecond);
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));
}

*/
