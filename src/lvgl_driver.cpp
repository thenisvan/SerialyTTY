#include "lvgl_driver.h"
#include "esp_log.h"
#include <cstring>

static const char* TAG = "LVGL_DRV";

// Static instance for callbacks
LVGLDriver* LVGLDriver::instance = nullptr;

LVGLDriver::LVGLDriver() : 
    display(nullptr),
    tft(nullptr),
    draw_buf(nullptr)
{
    instance = this;
}

LVGLDriver::~LVGLDriver() {
    if (draw_buf) {
        free(draw_buf);
    }
    instance = nullptr;
}

bool LVGLDriver::begin(TFTDriver* tftDriver) {
    if (!tftDriver) {
        ESP_LOGE(TAG, "TFT driver is NULL");
        return false;
    }
    
    tft = tftDriver;
    
    // Initialize LVGL
    lv_init();
    ESP_LOGI(TAG, "LVGL initialized");
    
    // Allocate draw buffer
    draw_buf = (lv_color_t*)heap_caps_malloc(DRAW_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (!draw_buf) {
        ESP_LOGE(TAG, "Failed to allocate draw buffer");
        return false;
    }
    ESP_LOGI(TAG, "Draw buffer allocated: %d bytes", DRAW_BUF_SIZE * sizeof(lv_color_t));
    
    // Create display
    display = lv_display_create(320, 240);  // Adjust for your display size
    if (!display) {
        ESP_LOGE(TAG, "Failed to create display");
        free(draw_buf);
        draw_buf = nullptr;
        return false;
    }
    
    // Set display buffer
    lv_display_set_buffers(display, draw_buf, nullptr, DRAW_BUF_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    // Set flush callback
    lv_display_set_flush_cb(display, flushCallback);
    
    // Set display color format
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    
    ESP_LOGI(TAG, "LVGL driver initialized successfully");
    return true;
}

void LVGLDriver::tick() {
    lv_tick_inc(10);  // Tell LVGL that 10ms have elapsed
    lv_timer_handler();  // Handle LVGL tasks
}

lv_display_t* LVGLDriver::getDisplay() {
    return display;
}

void LVGLDriver::flushCallback(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
    if (!instance || !instance->tft) {
        lv_display_flush_ready(disp);
        return;
    }
    
    // Get pixel data
    lv_color_t* color_p = (lv_color_t*)px_map;
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    // Send to TFT display
    instance->tft->setWindow(area->x1, area->y1, area->x2, area->y2);
    
    // Push pixels
    for (uint32_t i = 0; i < w * h; i++) {
        instance->tft->pushColor(color_p[i].red, color_p[i].green, color_p[i].blue);
    }
    
    // Inform LVGL that flushing is done
    lv_display_flush_ready(disp);
}

lv_obj_t* LVGLDriver::createRealtimeChart(lv_obj_t* parent, int width, int height) {
    // Create chart object
    lv_obj_t* chart = lv_chart_create(parent ? parent : lv_screen_active());
    lv_obj_set_size(chart, width, height);
    lv_obj_center(chart);
    
    // Configure chart
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 100);  // Keep last 100 points
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    
    // Add a data series
    lv_chart_series_t* ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    
    // Set update mode to shift (circular buffer)
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    
    ESP_LOGI(TAG, "Chart created: %dx%d", width, height);
    return chart;
}

void LVGLDriver::addChartDataPoint(lv_obj_t* chart, uint8_t seriesIdx, int32_t value) {
    if (!chart) return;
    
    // Get series
    lv_chart_series_t* ser = lv_chart_get_series_next(chart, nullptr);
    
    // Skip to the right series
    for (uint8_t i = 0; i < seriesIdx && ser; i++) {
        ser = lv_chart_get_series_next(chart, ser);
    }
    
    if (ser) {
        lv_chart_set_next_value(chart, ser, value);
    }
}

lv_obj_t* LVGLDriver::createLabel(lv_obj_t* parent, const char* text) {
    lv_obj_t* label = lv_label_create(parent ? parent : lv_screen_active());
    lv_label_set_text(label, text);
    return label;
}

void LVGLDriver::updateLabel(lv_obj_t* label, const char* text) {
    if (label) {
        lv_label_set_text(label, text);
    }
}
