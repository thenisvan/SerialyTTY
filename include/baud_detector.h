#ifndef BAUD_DETECTOR_H
#define BAUD_DETECTOR_H

#include <HardwareSerial.h>
#include "config.h"

class BaudDetector {
private:
    HardwareSerial* serial;
    uint8_t rxPin;
    uint8_t txPin;
    
public:
    BaudDetector(uint8_t rx, uint8_t tx);
    void begin();
    
    // Detect baud rate by analyzing incoming data
    uint32_t detectBaudRate(unsigned long timeout = DETECTION_TIMEOUT);
    
    // Test if data is valid at given baud rate
    bool testBaudRate(uint32_t baud, unsigned long timeout = 500);
    
    // Check if data is coming in
    bool isDataAvailable();
    
    // Get detected baud rate
    uint32_t getDetectedBaud() { return detectedBaud; }
    
private:
    uint32_t detectedBaud;
    
    // Analyze bit timing patterns
    float analyzeBitPattern();
    
    // Try common baud rates
    uint32_t tryCommonBaudRates();
};

#endif // BAUD_DETECTOR_H

