#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <SD.h>
#include <SPI.h>
#include "config.h"

class SDLogger {
private:
    bool sdInitialized;
    File logFile;
    unsigned long logCounter;
    
public:
    SDLogger();
    bool begin();
    void log(String message);
    void logData(String direction, String data);
    void logBaudDetection(uint32_t baud);
    void logStateChange(SystemState state);
    void logTestResult(bool success);
    void flush();
    void close();
    
private:
    String getFileName();
    void ensureSDReady();
};

#endif // SD_LOGGER_H

