#ifndef LVGL_DRIVER_H
#define LVGL_DRIVER_H

#include "lvgl.h"
#include "tft_driver.h"
#include <stdint.h>

class LVGLDriver {
public:
    LVGLDriver();
    ~LVGLDriver();
    
    /**
     * Initialize LVGL with TFT display
     * @param tftDriver Pointer to initialized TFT driver
     * @return true if successful
     */
    bool begin(TFTDriver* tftDriver);
    
    /**
     * Must be called periodically to handle LVGL tasks
     * Call this in your main loop
     */
    void tick();
    
    /**
     * Get the main display object
     * @return Pointer to lv_display_t
     */
    lv_display_t* getDisplay();
    
    /**
     * Create a real-time data chart
     * @param parent Parent object (NULL for screen)
     * @param width Chart width in pixels
     * @param height Chart height in pixels
     * @return Pointer to chart object
     */
    lv_obj_t* createRealtimeChart(lv_obj_t* parent, int width, int height);
    
    /**
     * Add data point to chart series
     * @param chart Chart object
     * @param seriesIdx Series index (0 for first series)
     * @param value Data value
     */
    void addChartDataPoint(lv_obj_t* chart, uint8_t seriesIdx, int32_t value);
    
    /**
     * Create a status label
     * @param parent Parent object
     * @param text Initial text
     * @return Pointer to label object
     */
    lv_obj_t* createLabel(lv_obj_t* parent, const char* text);
    
    /**
     * Update label text
     * @param label Label object
     * @param text New text
     */
    void updateLabel(lv_obj_t* label, const char* text);

private:
    lv_display_t* display;
    TFTDriver* tft;
    lv_color_t* draw_buf;
    
    static const uint32_t DRAW_BUF_SIZE = 320 * 10; // Buffer for 10 lines
    
    // Static callback for display flush
    static void flushCallback(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);
    
    // Instance pointer for callbacks
    static LVGLDriver* instance;
};

#endif // LVGL_DRIVER_H
