#ifndef COMM_TESTER_H
#define COMM_TESTER_H

#include <HardwareSerial.h>
#include "config.h"

class CommTester {
private:
    HardwareSerial* serial;
    uint32_t baudRate;
    unsigned long lastTestTime;
    bool lastTestResult;
    
public:
    CommTester();
    void begin(uint32_t baud, uint8_t rx, uint8_t tx);
    bool testCommunication();
    void sendTestMessage();
    bool waitForResponse(unsigned long timeout = RESPONSE_TIMEOUT);
    bool getLastTestResult() { return lastTestResult; }
    
private:
    String testMessage;
    void resetSerial();
};

#endif // COMM_TESTER_H

