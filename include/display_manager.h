#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

class DisplayManager {
private:
    SystemState currentState;
    char statusText[128];
    char baudText[32];
    char dataText[128];
    bool needsUpdate;
    bool present;
    
public:
    DisplayManager();
    bool begin();
    void clear();
    void update();
    void setState(SystemState state);
    void setBaudRate(uint32_t baud);
    void addData(const char* data);
    void setStatus(const char* status);
    bool isPresent();
    
private:
    void drawBootScreen();
    void drawWaitingScreen();
    void drawAnalyzingScreen();
    void drawFoundSpeedScreen();
    void drawRestartNeededScreen();
    void drawTestingScreen();
    void drawRunningScreen();
    const char* getStateName(SystemState state);
};

#endif // DISPLAY_MANAGER_H

