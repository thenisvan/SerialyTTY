#include "sd_logger.h"

SDLogger::SDLogger() {
    sdInitialized = false;
    logCounter = 0;
}

bool SDLogger::begin() {
    if (!SD.begin(SD_CS)) {
        sdInitialized = false;
        return false;
    }
    
    sdInitialized = true;
    log("SD Card initialized");
    return true;
}

void SDLogger::log(String message) {
    if (!sdInitialized) return;
    
    ensureSDReady();
    
    String logEntry = String(millis()) + " [" + String(logCounter++) + "] " + message;
    Serial.println(logEntry);
    
    if (logFile) {
        logFile.println(logEntry);
        flush();
    }
}

void SDLogger::logData(String direction, String data) {
    String entry = direction + " DATA: " + data;
    log(entry);
}

void SDLogger::logBaudDetection(uint32_t baud) {
    String entry = "BAUD DETECTED: " + String(baud) + " bps";
    log(entry);
}

void SDLogger::logStateChange(SystemState state) {
    String stateName;
    switch(state) {
        case STATE_BOOTING: stateName = "BOOTING"; break;
        case STATE_WAITING: stateName = "WAITING"; break;
        case STATE_ANALYZING: stateName = "ANALYZING"; break;
        case STATE_FOUND_SPEED: stateName = "FOUND_SPEED"; break;
        case STATE_RESTART_NEEDED: stateName = "RESTART_NEEDED"; break;
        case STATE_TESTING: stateName = "TESTING"; break;
        case STATE_RUNNING: stateName = "RUNNING"; break;
        default: stateName = "UNKNOWN"; break;
    }
    
    log("STATE: " + stateName);
}

void SDLogger::logTestResult(bool success) {
    String result = success ? "SUCCESS" : "FAILED";
    log("TEST: " + result);
}

void SDLogger::flush() {
    if (logFile) {
        logFile.flush();
    }
}

void SDLogger::close() {
    if (logFile) {
        logFile.close();
    }
}

void SDLogger::ensureSDReady() {
    if (!logFile) {
        logFile = SD.open(getFileName(), FILE_WRITE);
        if (!logFile) {
            Serial.println("Failed to open log file!");
        }
    }
}

String SDLogger::getFileName() {
    // Create filename with date/time
    unsigned long time = millis();
    return "LOG_" + String(time) + ".txt";
}

