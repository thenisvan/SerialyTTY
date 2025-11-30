#ifndef BAUD_DETECTOR_H
#define BAUD_DETECTOR_H

#include <stdint.h>
#include "config.h"

class BaudDetector {
private:
    uint32_t detectedBaud;
    
public:
    BaudDetector();
    void begin();
    
    // Detect baud rate by analyzing incoming data
    uint32_t detectBaudRate(uint32_t timeout = DETECTION_TIMEOUT);
    
    // Test if data is valid at given baud rate
    bool testBaudRate(uint32_t baud, uint32_t timeout = 500);
    
    // Check if data is coming in
    bool isDataAvailable();
    
    // Get detected baud rate
    uint32_t getDetectedBaud() { return detectedBaud; }
    
private:
    // Analyze bit timing patterns
    float analyzeBitPattern();
    
    // Try common baud rates
    uint32_t tryCommonBaudRates();
};

#endif // BAUD_DETECTOR_H
