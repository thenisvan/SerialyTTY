#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include "config.h"

#if defined(ARDUINO) || defined(ARDUINO_ARCH_ESP32)
#include <SD.h>
#include <SPI.h>
#include <Arduino.h>
#else
#include <string>
using String = std::string;
class File {};  // Minimal placeholder
#endif

class SDLogger {
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
    bool sdInitialized = false;
    File logFile;
    unsigned long logCounter = 0;
    const char* getFileName();
    void ensureSDReady();
};

#endif // SD_LOGGER_H

