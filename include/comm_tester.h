#ifndef COMM_TESTER_H
#define COMM_TESTER_H

#include "config.h"

#if defined(ARDUINO) || defined(ARDUINO_ARCH_ESP32)
#include <HardwareSerial.h>
#include <Arduino.h>
#else
#include <string>
#include <cstdint>
using String = std::string;
// Forward declare only if not already declared by baud_detector.h
#ifndef HARDWARESERIAL_COMPAT_H
#define HARDWARESERIAL_COMPAT_H
class HardwareSerial {};
#endif
#endif

class CommTester {
public:
    CommTester();
    // ESP-IDF implementation uses begin(baud). Arduino variant may provide
    // pin parameters. Provide both overloads for compatibility.
    void begin(uint32_t baud, uint8_t rx, uint8_t tx);
    void begin(uint32_t baud);
    bool testCommunication();
    void sendTestMessage();
    bool waitForResponse(uint32_t timeout = RESPONSE_TIMEOUT);
    bool getLastTestResult() { return lastTestResult; }

private:
    HardwareSerial* serial = nullptr;
    uint32_t baudRate = 0;
    unsigned long lastTestTime = 0;
    bool lastTestResult = false;
    String testMessage;
    void resetSerial();
};

#endif // COMM_TESTER_H

