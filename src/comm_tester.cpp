// Arduino-style implementation. Compiled only when building with Arduino.
#if defined(ARDUINO) || defined(ARDUINO_ARCH_ESP32)

#include "comm_tester.h"

CommTester::CommTester() {
    lastTestTime = 0;
    lastTestResult = false;
    testMessage = TEST_MESSAGE;
}

void CommTester::begin(uint32_t baud, uint8_t rx, uint8_t tx) {
    baudRate = baud;
    Serial1.begin(baud, SERIAL_8N1, rx, tx);
    Serial.println("Communication tester initialized at " + String(baud) + " bps");
}

bool CommTester::testCommunication() {
    sendTestMessage();
    bool result = waitForResponse(RESPONSE_TIMEOUT);
    lastTestResult = result;
    lastTestTime = millis();
    return result;
}

void CommTester::sendTestMessage() {
    if (Serial1.available()) {
        // Clear RX buffer
        while (Serial1.available()) {
            Serial1.read();
        }
    }
    
    Serial1.print(testMessage);
    Serial.println("Sent test message: " + testMessage);
}

bool CommTester::waitForResponse(unsigned long timeout) {
    unsigned long startTime = millis();
    
    while (millis() - startTime < timeout) {
        if (Serial1.available()) {
            // Got response!
            Serial.println("Device responded!");
            return true;
        }
    }
    
    Serial.println("No response from device");
    return false;
}

void CommTester::resetSerial() {
    Serial1.end();
    delay(10);
}

#endif // ARDUINO

