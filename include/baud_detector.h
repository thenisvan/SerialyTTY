#ifndef BAUD_DETECTOR_H
#define BAUD_DETECTOR_H

#include "config.h"

#if defined(ARDUINO) || defined(ARDUINO_ARCH_ESP32)
#include <HardwareSerial.h>
#include <Arduino.h>
#else
#include <string>
#include <cstdint>
// Minimal compatibility types for ESP-IDF builds
using String = std::string;
// Forward-declare a lightweight HardwareSerial placeholder type to keep headers
// compatible. Implementations for ESP-IDF should not rely on this type.
#ifndef HARDWARESERIAL_COMPAT_H
#define HARDWARESERIAL_COMPAT_H
class HardwareSerial {};
#endif
#endif

class BaudDetector {
public:
    // For Arduino build the constructor accepts pins; ESP-IDF implementation
    // may provide a default constructor. Keep both declarations compatible.
    BaudDetector(uint8_t rx, uint8_t tx);
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
    // Hardware-specific serial pointer (may be null for ESP-IDF implementations)
    HardwareSerial* serial = nullptr;
    uint8_t rxPin = 0;
    uint8_t txPin = 0;
    uint32_t detectedBaud = 0;

    // Analyze bit timing patterns
    float analyzeBitPattern();

    // Try common baud rates
    uint32_t tryCommonBaudRates();
};

#endif // BAUD_DETECTOR_H

