// This file contains the Arduino-style implementation. It should only be
// compiled when building for the Arduino framework. For ESP-IDF builds,
// `baud_detector_espidf.cpp` provides the implementation.
#if defined(ARDUINO) || defined(ARDUINO_ARCH_ESP32)

#include "baud_detector.h"

BaudDetector::BaudDetector(uint8_t rx, uint8_t tx) : rxPin(rx), txPin(tx) {
    detectedBaud = 0;
}

void BaudDetector::begin() {
    // Initialize serial for receiving
    // We'll use SoftwareSerial if needed or configure HardwareSerial
}

bool BaudDetector::isDataAvailable() {
    // Simple check - look for transitions on RX pin
    static int lastState = digitalRead(rxPin);
    int currentState = digitalRead(rxPin);
    
    if (lastState != currentState) {
        lastState = currentState;
        return true;
    }
    return false;
}

uint32_t BaudDetector::detectBaudRate(unsigned long timeout) {
    unsigned long startTime = millis();
    detectedBaud = 0;
    
    // Wait for initial data
    bool gotData = false;
    while (millis() - startTime < 100 && !gotData) {
        if (isDataAvailable()) {
            gotData = true;
            delay(50); // Let some data accumulate
        }
    }
    
    if (!gotData) {
        return 0; // No data detected
    }
    
    // Try each common baud rate
    for (size_t i = 0; i < NUM_BAUDRATES; i++) {
        uint32_t baud = BAUDRATES[i];
        if (testBaudRate(baud, timeout / NUM_BAUDRATES)) {
            detectedBaud = baud;
            return baud;
        }
    }
    
    return 0;
}

bool BaudDetector::testBaudRate(uint32_t baud, unsigned long timeout) {
    Serial1.end();
    delay(10);
    Serial1.begin(baud, SERIAL_8N1, rxPin, txPin);
    
    unsigned long startTime = millis();
    unsigned long lastCharTime = millis();
    int byteCount = 0;
    bool gotValidData = false;
    
    // Monitor for incoming data that makes sense
    while (millis() - startTime < timeout) {
        if (Serial1.available()) {
            int c = Serial1.read();
            if (c >= 0) {
                byteCount++;
                lastCharTime = millis();
                gotValidData = true;
            }
        }
        
        // If we got several bytes in quick succession, this might be the right baud
        if (byteCount > 5 && (millis() - lastCharTime) < 100) {
            return true;
        }
    }
    
    return gotValidData && byteCount > 3;
}

float BaudDetector::analyzeBitPattern() {
    // Advanced: measure bit transition timing
    // This is more complex and would require microsecond precision timing
    return 0.0;
}

uint32_t BaudDetector::tryCommonBaudRates() {
    detectedBaud = detectBaudRate(2000);
    return detectedBaud;
}

#endif // ARDUINO

