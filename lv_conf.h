/**
 * @file lv_conf.h
 * Configuration file for LVGL v9.2
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

/* Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888) */
#define LV_COLOR_DEPTH 16

/*=========================
   MEMORY SETTINGS
 *=========================*/

/* Size of the memory available for `lv_malloc()` in bytes (>= 2kB) */
#define LV_MEM_SIZE (64U * 1024U)  /* 64 KB */

/* Set an address for the memory pool instead of allocating it as a normal array */
#define LV_MEM_ADR 0     /* 0: unused */

/* Memory pooling: allocate memory in fixed-size chunks */
#define LV_MEM_POOL_INCLUDE <stdlib.h>
#define LV_MEM_POOL_ALLOC malloc
#define LV_MEM_POOL_FREE free

/*====================
   HAL SETTINGS
 *====================*/

/* Default display refresh period in milliseconds */
#define LV_DEF_REFR_PERIOD 30  /* 33 FPS */

/* Input device read period in milliseconds */
#define LV_INDEV_DEF_READ_PERIOD 30

/* Use a custom tick source instead of lv_tick_inc() */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "freertos/FreeRTOS.h"
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (xTaskGetTickCount() * portTICK_PERIOD_MS)
#endif

/* DMA or GPU support for accelerated rendering */
#define LV_USE_GPU_SDL 0

/*================
 * LOG SETTINGS
 *================*/

/* Enable the log module */
#define LV_USE_LOG 0
#if LV_USE_LOG
    /* How important log should be added:
     * LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
     * LV_LOG_LEVEL_INFO        Log important events
     * LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
     * LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
     * LV_LOG_LEVEL_USER        Only logs added by the user
     * LV_LOG_LEVEL_NONE        Do not log anything */
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

    /* Print the log with printf */
    #define LV_LOG_PRINTF 1
    #if LV_LOG_PRINTF
        #include "esp_log.h"
        #define LV_LOG_TRACE(...)  ESP_LOGD("LVGL", __VA_ARGS__)
        #define LV_LOG_INFO(...)   ESP_LOGI("LVGL", __VA_ARGS__)
        #define LV_LOG_WARN(...)   ESP_LOGW("LVGL", __VA_ARGS__)
        #define LV_LOG_ERROR(...)  ESP_LOGE("LVGL", __VA_ARGS__)
        #define LV_LOG_USER(...)   ESP_LOGI("LVGL", __VA_ARGS__)
    #endif
#endif

/*=================
 * FONT USAGE
 *=================*/

/* Montserrat fonts with various sizes */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/* Default font */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/*===================
 * WIDGET USAGE
 *===================*/

/* Documentation of the widgets: https://docs.lvgl.io/latest/en/html/widgets/index.html */

#define LV_USE_ANIMIMG    1
#define LV_USE_ARC        1
#define LV_USE_BAR        1
#define LV_USE_BTN        1
#define LV_USE_BTNMATRIX  1
#define LV_USE_CALENDAR   0
#define LV_USE_CANVAS     1
#define LV_USE_CHART      1  /* Enable charts for graphing */
#define LV_USE_CHECKBOX   1
#define LV_USE_DROPDOWN   1
#define LV_USE_IMG        1
#define LV_USE_LABEL      1
#define LV_USE_LED        1
#define LV_USE_LINE       1
#define LV_USE_LIST       1
#define LV_USE_MENU       1
#define LV_USE_METER      1
#define LV_USE_MSGBOX     1
#define LV_USE_ROLLER     1
#define LV_USE_SCALE      1
#define LV_USE_SLIDER     1
#define LV_USE_SPAN       1
#define LV_USE_SPINBOX    1
#define LV_USE_SPINNER    1
#define LV_USE_SWITCH     1
#define LV_USE_TABLE      1
#define LV_USE_TABVIEW    1
#define LV_USE_TEXTAREA   1
#define LV_USE_TILEVIEW   1
#define LV_USE_WIN        1

/*==================
 * CHART SETTINGS
 *==================*/

#if LV_USE_CHART
    /* Maximum number of data series per chart */
    #define LV_CHART_POINT_COUNT_MAX 256
#endif

/*==================
 * THEMES
 *==================*/

/* A simple, impressive and very complete theme */
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    /* 0: Light mode; 1: Dark mode */
    #define LV_THEME_DEFAULT_DARK 0

    /* 1: Enable grow on press */
    #define LV_THEME_DEFAULT_GROW 1

    /* Default transition time in [ms] */
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif

/* A very simple theme that is a good starting point for a custom theme */
#define LV_USE_THEME_BASIC 1

/* A theme designed for monochrome displays */
#define LV_USE_THEME_MONO 0

/*==================
 * LAYOUTS
 *==================*/

/* A layout similar to Flexbox in CSS */
#define LV_USE_FLEX 1

/* A layout similar to Grid in CSS */
#define LV_USE_GRID 1

/*==================
 * OTHERS
 *==================*/

/* Use snapshot for screen capture */
#define LV_USE_SNAPSHOT 0

/* Use FreeType for font rendering */
#define LV_USE_FREETYPE 0

/* Use TinyTTF for TTF font rendering */
#define LV_USE_TINY_TTF 0

/* Use RLE compression for images */
#define LV_USE_RLE 0

/* Use PNG decoder */
#define LV_USE_PNG 0

/* Use BMP decoder */
#define LV_USE_BMP 0

/* Use JPG decoder */
#define LV_USE_SJPG 0

/* Use GIF decoder */
#define LV_USE_GIF 0

/* Use QR code generator */
#define LV_USE_QRCODE 0

/* Use Barcode generator */
#define LV_USE_BARCODE 0

/*==================
 * EXAMPLES
 *==================*/

/* Enable examples (disable to save space) */
#define LV_BUILD_EXAMPLES 0

/*==================
 * DEMOS
 *==================*/

/* Show some widgets (disable to save space) */
#define LV_USE_DEMO_WIDGETS 0

/* Demonstrate the usage of encoder and keyboard */
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0

/* Benchmark your system */
#define LV_USE_DEMO_BENCHMARK 0

/* Stress test for memory and performance */
#define LV_USE_DEMO_STRESS 0

/* Music player demo */
#define LV_USE_DEMO_MUSIC 0

#endif /*LV_CONF_H*/
