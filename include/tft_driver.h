#ifndef TFT_DRIVER_H
#define TFT_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

// TFT ILI9341 Display Driver for ESP-IDF
// 240x320 pixel color display with SPI interface

// Default SPI pins for ESP32-C6
#define TFT_MOSI_PIN    GPIO_NUM_7
#define TFT_MISO_PIN    GPIO_NUM_2
#define TFT_SCLK_PIN    GPIO_NUM_6
#define TFT_CS_PIN      GPIO_NUM_10
#define TFT_DC_PIN      GPIO_NUM_3   // Data/Command pin
#define TFT_RST_PIN     GPIO_NUM_4   // Reset pin
#define TFT_BL_PIN      GPIO_NUM_5   // Backlight pin

// Display dimensions
#define TFT_WIDTH       240
#define TFT_HEIGHT      320

// Color definitions (RGB565 format)
#define TFT_BLACK       0x0000
#define TFT_WHITE       0xFFFF
#define TFT_RED         0xF800
#define TFT_GREEN       0x07E0
#define TFT_BLUE        0x001F
#define TFT_CYAN        0x07FF
#define TFT_MAGENTA     0xF81F
#define TFT_YELLOW      0xFFE0
#define TFT_ORANGE      0xFD20
#define TFT_GRAY        0x8410
#define TFT_DARKGRAY    0x4208

// ILI9341 Commands
#define ILI9341_NOP         0x00
#define ILI9341_SWRESET     0x01
#define ILI9341_SLPOUT      0x11
#define ILI9341_DISPON      0x29
#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C
#define ILI9341_MADCTL      0x36
#define ILI9341_PIXFMT      0x3A

class TFTDriver {
private:
    spi_device_handle_t spi;
    bool initialized;
    uint16_t width;
    uint16_t height;
    uint8_t rotation;
    
    // Font rendering
    static const uint8_t font8x8[][8];
    
    // Low-level functions
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void writeData16(uint16_t data);
    void writeData32(uint32_t data);
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    
public:
    TFTDriver();
    ~TFTDriver();
    
    // Initialization
    bool begin();
    void reset();
    void setRotation(uint8_t r);
    void setBacklight(bool on);
    
    // Basic drawing
    void fillScreen(uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    
    // Text rendering
    void drawChar(int16_t x, int16_t y, char c, uint16_t fg, uint16_t bg);
    void drawString(int16_t x, int16_t y, const char* str, uint16_t fg, uint16_t bg);
    void drawStringCentered(int16_t y, const char* str, uint16_t fg, uint16_t bg);
    
    // Utility
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    bool isInitialized() const { return initialized; }
};

#endif // TFT_DRIVER_H
