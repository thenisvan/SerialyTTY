#ifndef BRIDGE_MODE_H
#define BRIDGE_MODE_H

#include <cstdint>
#include "driver/uart.h"

// Forward declarations
class DisplayManager;
class SDLogger;

// Escape sequence detection
#define ESCAPE_SEQUENCE "~~~"
#define ESCAPE_SEQ_LEN 3
#define ESCAPE_TIMEOUT_MS 1000  // Time window to receive full sequence

class BridgeMode {
public:
    BridgeMode();
    
    // Bridge mode control
    void enter(uint32_t baudRate);
    void exit();
    bool isActive() const { return active; }
    
    // Process data in bridge mode
    void handleData();
    
    // Statistics
    uint32_t getBytesRx() const { return bytesReceived; }
    uint32_t getBytesTx() const { return bytesSent; }
    void resetStats();
    
    // Set optional components
    void setDisplay(DisplayManager* disp) { display = disp; }
    void setLogger(SDLogger* logger) { sdLogger = logger; }

private:
    bool active;
    uint32_t currentBaud;
    
    // Statistics
    uint32_t bytesReceived;
    uint32_t bytesSent;
    
    // Escape sequence detection
    char escapeBuffer[ESCAPE_SEQ_LEN];
    uint8_t escapeIndex;
    uint64_t lastEscapeChar;
    
    // Optional components
    DisplayManager* display;
    SDLogger* sdLogger;
    
    bool checkEscapeSequence(char c);
    void resetEscapeDetection();
    
    // USB CDC Serial (UART_NUM_0)
    void forwardUsbToTarget(const uint8_t* data, size_t len);
    void forwardTargetToUsb(const uint8_t* data, size_t len);
};

#endif // BRIDGE_MODE_H
