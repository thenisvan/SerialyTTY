#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>

// Hardware Configuration
#define RX_PIN 4
#define TX_PIN 5

// Display Configuration (TFT eSPI)
#define TFT_CS    10
#define TFT_DC    8
#define TFT_RST   -1  // Connected to reset
#define TFT_MOSI  11
#define TFT_SCK   12
#define TFT_MISO  13

// SD Card Configuration
#define SD_CS      9
#define SD_CS_PIN  SD_CS  // Alias for hardware detector

// I2C Configuration (for hardware detection)
#define I2C_SDA_PIN  6
#define I2C_SCL_PIN  7
#define I2C_FREQ_HZ  100000  // 100kHz

// Baud Rate Detection
#define MIN_BAUD_RATE    9600
#define MAX_BAUD_RATE    115200
#define DETECTION_TIMEOUT 2000  // ms
#define SAMPLE_SIZE       128

// Testing Configuration
#define TEST_MESSAGE     "AT\r\n"
#define RESPONSE_TIMEOUT 1000  // ms

// Logging
#define LOG_BUFFER_SIZE  256
#define MAX_LOG_LINES    1000

// Common Baud Rates to Test
static const uint32_t BAUDRATES[] = {
    9600,
    19200,
    28800,
    38400,
    57600,
    115200
};

#define NUM_BAUDRATES sizeof(BAUDRATES)/sizeof(BAUDRATES[0])

// System States
enum SystemState {
    STATE_BOOTING,
    STATE_WAITING,
    STATE_ANALYZING,
    STATE_FOUND_SPEED,
    STATE_RESTART_NEEDED,
    STATE_TESTING,
    STATE_RUNNING,
    STATE_BRIDGE_MODE,
    STATE_MENU
};

#endif // CONFIG_H

