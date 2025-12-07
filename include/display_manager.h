#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "tft_driver.h"

class DisplayManager {
private:
    TFTDriver* tft;
    SystemState currentState;
    char statusText[128];
    char baudText[32];
    char dataText[128];
    uint32_t rxBytes;
    uint32_t txBytes;
    bool needsUpdate;
    bool present;
    
public:
    DisplayManager();
    ~DisplayManager();
    bool begin();
    void clear();
    void update();
    void setState(SystemState state);
    void setBaudRate(uint32_t baud);
    void setDataStats(uint32_t rx, uint32_t tx);
    void addData(const char* data);
    void setStatus(const char* status);
    bool isPresent();
    
private:
    void drawHeader(const char* title);
    void drawStatusBar();
    void drawBootScreen();
    void drawMenuScreen();
    void drawAnalyzingScreen();
    void drawBridgeScreen();
    const char* getStateName(SystemState state);
};

#endif // DISPLAY_MANAGER_H

