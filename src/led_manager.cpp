#include "led_manager.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "LED";

static uint32_t millis() {
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

LedManager::LedManager() : led_strip(NULL), currentState(STATE_BOOTING), lastUpdate(0), blinkState(false) {
}

bool LedManager::begin() {
    ESP_LOGI(TAG, "Initializing LED strip on GPIO %d", LED_STRIP_GPIO_PIN);
    
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_PIN,
        .max_leds = LED_STRIP_MAX_LEDS,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags = { .invert_out = false },
    };
    
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags = { .with_dma = false },
    };
    
    esp_err_t ret = led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create LED strip device: %s", esp_err_to_name(ret));
        return false;
    }
    
    led_strip_clear(led_strip);
    return true;
}

void LedManager::setColor(uint8_t r, uint8_t g, uint8_t b) {
    if (!led_strip) return;
    led_strip_set_pixel(led_strip, 0, r, g, b);
    led_strip_refresh(led_strip);
}

void LedManager::setState(SystemState state) {
    currentState = state;
    update();
}

void LedManager::update() {
    if (!led_strip) return;
    
    uint32_t now = millis();
    
    switch (currentState) {
        case STATE_BOOTING:
            setColor(20, 20, 20); // White dim
            break;
            
        case STATE_WAITING:
            // Blink Blue
            if (now - lastUpdate > 500) {
                blinkState = !blinkState;
                lastUpdate = now;
                if (blinkState) setColor(0, 0, 20);
                else setColor(0, 0, 0);
            }
            break;
            
        case STATE_ANALYZING:
            // Fast Blink Yellow
            if (now - lastUpdate > 100) {
                blinkState = !blinkState;
                lastUpdate = now;
                if (blinkState) setColor(20, 20, 0);
                else setColor(0, 0, 0);
            }
            break;
            
        case STATE_FOUND_SPEED:
            setColor(0, 20, 0); // Green
            break;
            
        case STATE_RUNNING:
            setColor(0, 20, 20); // Cyan
            break;
            
        case STATE_BRIDGE_MODE:
            setColor(20, 0, 20); // Magenta
            break;
            
        case STATE_MENU:
            setColor(10, 0, 20); // Purple dim
            break;
            
        case STATE_RESTART_NEEDED:
            // Blink Red
            if (now - lastUpdate > 250) {
                blinkState = !blinkState;
                lastUpdate = now;
                if (blinkState) setColor(20, 0, 0);
                else setColor(0, 0, 0);
            }
            break;
            
        case STATE_TESTING:
             // Blink Orange
            if (now - lastUpdate > 200) {
                blinkState = !blinkState;
                lastUpdate = now;
                if (blinkState) setColor(20, 10, 0);
                else setColor(0, 0, 0);
            }
            break;

        default:
            setColor(0, 0, 0); // Off
            break;
    }
}
