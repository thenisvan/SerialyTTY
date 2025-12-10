#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include "driver/gpio.h"
#include "led_strip.h"
#include "config.h"

// Default LED pin for ESP32-C6 DevKitC-1
#define LED_STRIP_GPIO_PIN GPIO_NUM_8
#define LED_STRIP_MAX_LEDS 1

class LedManager {
public:
    LedManager();
    bool begin();
    void setState(SystemState state);
    void update(); // Call in main loop for blinking effects

private:
    led_strip_handle_t led_strip;
    SystemState currentState;
    uint32_t lastUpdate;
    bool blinkState;
    float breathPhase;
    
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    float getBreatheIntensity();
};

#endif
