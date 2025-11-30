#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

class SDLogger {
private:
    bool sdInitialized;
    uint32_t logCounter;
    
public:
    SDLogger();
    bool begin();
    void log(const char* message);
    void logData(const char* direction, const char* data);
    void logBaudDetection(uint32_t baud);
    void logStateChange(SystemState state);
    void logTestResult(bool success);
    void flush();
    void close();
    
private:
    const char* getFileName();
    void ensureSDReady();
};

#endif // SD_LOGGER_H
