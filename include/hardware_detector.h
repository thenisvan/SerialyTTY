#ifndef HARDWARE_DETECTOR_H
#define HARDWARE_DETECTOR_H

#include <cstdint>
#include "driver/i2c.h"
#include "driver/gpio.h"

// Hardware detection results
struct HardwareConfig {
    bool displayPresent;
    uint8_t displayAddress;
    bool sdCardPresent;
    bool accelerometerPresent;
    uint8_t accelAddress;
};

class HardwareDetector {
public:
    HardwareDetector();
    
    // Scan for all hardware modules
    HardwareConfig scanAll();
    
    // Individual detection methods
    bool detectDisplay();
    bool detectSDCard();
    bool detectAccelerometer();
    
    // Get last scan results
    const HardwareConfig& getConfig() const { return config; }
    
    // I2C utilities
    bool scanI2CBus(uint8_t* foundDevices, size_t maxDevices);
    bool checkI2CDevice(uint8_t address);

private:
    HardwareConfig config;
    bool i2cInitialized;
    
    void initI2C();
    void deinitI2C();
};

#endif // HARDWARE_DETECTOR_H
