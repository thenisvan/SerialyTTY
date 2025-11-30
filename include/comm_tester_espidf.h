#ifndef COMM_TESTER_H
#define COMM_TESTER_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

class CommTester {
private:
    uint32_t baudRate;
    uint32_t lastTestTime;
    bool lastTestResult;
    
public:
    CommTester();
    void begin(uint32_t baud);
    bool testCommunication();
    void sendTestMessage();
    bool waitForResponse(uint32_t timeout = RESPONSE_TIMEOUT);
    bool getLastTestResult() { return lastTestResult; }
    
private:
    const char* testMessage;
    void resetSerial();
};

#endif // COMM_TESTER_H
