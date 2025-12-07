#include "tft_driver.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "TFT";

// 8x8 bitmap font
const uint8_t TFTDriver::font8x8[][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Space (32)
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // !
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // "
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, // #
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00}, // $
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00}, // %
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00}, // &
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00}, // '
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00}, // (
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00}, // )
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00}, // *
    {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00}, // +
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // ,
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00}, // -
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // .
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00}, // /
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}, // 0
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00}, // 1
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00}, // 2
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00}, // 3
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00}, // 4
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00}, // 5
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00}, // 6
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00}, // 7
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00}, // 8
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00}, // 9
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // :
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // ;
    {0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00}, // <
    {0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00}, // =
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00}, // >
    {0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00}, // ?
    {0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00}, // @
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}, // A
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00}, // B
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00}, // C
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00}, // D
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00}, // E
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00}, // F
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00}, // G
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00}, // H
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // I
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00}, // J
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00}, // K
    {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00}, // L
    {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00}, // M
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00}, // N
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00}, // O
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00}, // P
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00}, // Q
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00}, // R
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00}, // S
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // T
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00}, // U
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // V
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // W
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00}, // X
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00}, // Y
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00}, // Z
};

TFTDriver::TFTDriver() : spi(nullptr), initialized(false), width(TFT_WIDTH), height(TFT_HEIGHT), rotation(0) {
}

TFTDriver::~TFTDriver() {
    if (spi != nullptr) {
        spi_bus_remove_device(spi);
        spi_bus_free(SPI2_HOST);
    }
}

bool TFTDriver::begin() {
    ESP_LOGI(TAG, "Initializing TFT display...");
    
    // Configure GPIO pins
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << TFT_DC_PIN) | (1ULL << TFT_RST_PIN) | (1ULL << TFT_BL_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    
    // Initialize SPI bus (shared with SD card, may already be initialized)
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = TFT_MOSI_PIN;
    buscfg.miso_io_num = TFT_MISO_PIN;
    buscfg.sclk_io_num = TFT_SCLK_PIN;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = TFT_WIDTH * TFT_HEIGHT * 2 + 8;
    
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "SPI bus initialization failed: %s", esp_err_to_name(ret));
        return false;
    }
    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGI(TAG, "SPI bus already initialized (shared with SD card)");
    }
    
    // Configure SPI device
    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = 40 * 1000 * 1000;  // 40 MHz
    devcfg.mode = 0;  // SPI mode 0
    devcfg.spics_io_num = TFT_CS_PIN;
    devcfg.queue_size = 7;
    devcfg.pre_cb = nullptr;
    devcfg.flags = 0;
    
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI device add failed: %s", esp_err_to_name(ret));
        return false;
    }
    
    // Hardware reset
    reset();
    
    // Try to initialize ILI9341 - if it fails, display is not present
    ESP_LOGI(TAG, "Attempting to initialize ILI9341 controller...");
    
    // Test if display responds by trying to read ID (simple presence check)
    // If CS pin is not connected or display is missing, this won't cause errors
    // since we're just doing a write-only initialization
    
    writeCommand(ILI9341_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));
    
    writeCommand(ILI9341_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    writeCommand(ILI9341_PIXFMT);
    writeData(0x55);  // 16-bit color
    
    writeCommand(ILI9341_MADCTL);
    writeData(0x48);  // MX, BGR
    
    writeCommand(ILI9341_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Turn on backlight
    setBacklight(true);
    
    // Clear screen to black - if display isn't present, this is harmless
    fillScreen(TFT_BLACK);
    
    initialized = true;
    ESP_LOGI(TAG, "TFT display initialized (or running without display)");
    return true;
}

void TFTDriver::reset() {
    gpio_set_level(TFT_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(TFT_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(150));
}

void TFTDriver::setBacklight(bool on) {
    gpio_set_level(TFT_BL_PIN, on ? 1 : 0);
}

void TFTDriver::writeCommand(uint8_t cmd) {
    gpio_set_level(TFT_DC_PIN, 0);  // Command mode
    spi_transaction_t t = {};
    t.length = 8;
    t.tx_buffer = &cmd;
    t.flags = 0;
    spi_device_polling_transmit(spi, &t);
}

void TFTDriver::writeData(uint8_t data) {
    gpio_set_level(TFT_DC_PIN, 1);  // Data mode
    spi_transaction_t t = {};
    t.length = 8;
    t.tx_buffer = &data;
    t.flags = 0;
    spi_device_polling_transmit(spi, &t);
}

void TFTDriver::writeData16(uint16_t data) {
    uint8_t buf[2] = {(uint8_t)(data >> 8), (uint8_t)(data & 0xFF)};
    gpio_set_level(TFT_DC_PIN, 1);  // Data mode
    spi_transaction_t t = {};
    t.length = 16;
    t.tx_buffer = buf;
    t.flags = 0;
    spi_device_polling_transmit(spi, &t);
}

void TFTDriver::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ILI9341_CASET);
    writeData16(x0);
    writeData16(x1);
    
    writeCommand(ILI9341_PASET);
    writeData16(y0);
    writeData16(y1);
    
    writeCommand(ILI9341_RAMWR);
}

void TFTDriver::fillScreen(uint16_t color) {
    if (!initialized) return;
    
    setAddrWindow(0, 0, width - 1, height - 1);
    
    gpio_set_level(TFT_DC_PIN, 1);  // Data mode
    
    uint32_t pixels = width * height;
    uint16_t colorBE = (color >> 8) | (color << 8);  // Swap bytes for big-endian SPI
    
    // Use DMA for large transfer
    uint16_t *buffer = (uint16_t *)heap_caps_malloc(4096 * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (buffer) {
        for (int i = 0; i < 4096; i++) {
            buffer[i] = colorBE;
        }
        
        spi_transaction_t t = {};
        t.length = 4096 * 16;
        t.tx_buffer = buffer;
        
        for (uint32_t i = 0; i < pixels / 4096; i++) {
            spi_device_polling_transmit(spi, &t);
        }
        
        // Handle remaining pixels
        uint32_t remaining = pixels % 4096;
        if (remaining > 0) {
            t.length = remaining * 16;
            spi_device_polling_transmit(spi, &t);
        }
        
        heap_caps_free(buffer);
    } else {
        // Fallback: pixel by pixel
        for (uint32_t i = 0; i < pixels; i++) {
            writeData16(color);
        }
    }
}

void TFTDriver::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (!initialized || x < 0 || x >= width || y < 0 || y >= height) return;
    
    setAddrWindow(x, y, x, y);
    writeData16(color);
}

void TFTDriver::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (!initialized || w <= 0 || h <= 0) return;
    
    // Clip to screen
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > width) w = width - x;
    if (y + h > height) h = height - y;
    if (w <= 0 || h <= 0) return;
    
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    
    gpio_set_level(TFT_DC_PIN, 1);  // Data mode
    uint32_t pixels = w * h;
    for (uint32_t i = 0; i < pixels; i++) {
        writeData16(color);
    }
}

void TFTDriver::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (!initialized) return;
    
    fillRect(x, y, w, 1, color);          // Top
    fillRect(x, y + h - 1, w, 1, color);  // Bottom
    fillRect(x, y, 1, h, color);          // Left
    fillRect(x + w - 1, y, 1, h, color);  // Right
}

void TFTDriver::drawChar(int16_t x, int16_t y, char c, uint16_t fg, uint16_t bg) {
    if (!initialized || c < 32 || c > 90) return;
    
    int index = c - 32;
    if (index < 0 || index >= (int)(sizeof(font8x8) / sizeof(font8x8[0]))) return;
    
    for (int row = 0; row < 8; row++) {
        uint8_t line = font8x8[index][row];
        for (int col = 0; col < 8; col++) {
            if (line & (1 << col)) {
                drawPixel(x + col, y + row, fg);
            } else {
                drawPixel(x + col, y + row, bg);
            }
        }
    }
}

void TFTDriver::drawString(int16_t x, int16_t y, const char* str, uint16_t fg, uint16_t bg) {
    if (!initialized || !str) return;
    
    int16_t cursorX = x;
    while (*str) {
        drawChar(cursorX, y, *str, fg, bg);
        cursorX += 8;
        str++;
    }
}

void TFTDriver::drawStringCentered(int16_t y, const char* str, uint16_t fg, uint16_t bg) {
    if (!initialized || !str) return;
    
    int len = strlen(str);
    int16_t x = (width - len * 8) / 2;
    drawString(x, y, str, fg, bg);
}

void TFTDriver::setRotation(uint8_t r) {
    rotation = r % 4;
    
    writeCommand(ILI9341_MADCTL);
    switch (rotation) {
        case 0:
            writeData(0x48);  // MX, BGR
            width = TFT_WIDTH;
            height = TFT_HEIGHT;
            break;
        case 1:
            writeData(0x28);  // MV, BGR
            width = TFT_HEIGHT;
            height = TFT_WIDTH;
            break;
        case 2:
            writeData(0x88);  // MY, BGR
            width = TFT_WIDTH;
            height = TFT_HEIGHT;
            break;
        case 3:
            writeData(0xE8);  // MX, MY, MV, BGR
            width = TFT_HEIGHT;
            height = TFT_WIDTH;
            break;
    }
}

uint16_t TFTDriver::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
