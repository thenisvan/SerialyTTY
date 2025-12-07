#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include "config.h"
#include <stdint.h>
#include <time.h>

class SDLogger {
public:
    SDLogger();
    ~SDLogger();
    bool begin();
    void log(const char* message);
    void logData(const char* direction, const uint8_t* data, size_t len);
    void logBaudDetection(uint32_t baud);
    void logStateChange(SystemState state);
    void logTestResult(bool success);
    void flush();
    void close();
    bool isReady() const { return sdInitialized; }
    uint32_t getLogCount() const { return logCounter; }

private:
    bool sdInitialized;
    uint32_t logCounter;
    char currentLogFile[64];
    void* fileHandle;  // Opaque handle for FILE*
    
    void ensureSDReady();
    void openLogFile();
    void writeLog(const char* level, const char* message);
    const char* getStateName(SystemState state);
    void getTimestamp(char* buffer, size_t size);
};

#endif // SD_LOGGER_H

