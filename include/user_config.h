// Debug
#ifndef DEBUG_PRINT_MODE
#define DEBUG_PRINT_MODE
#endif

// UART
#define	BITRATE 115200

// Network
#define SSID     "mrrb_dev"
#define SSID_PW  "WhatTimeIsIt?AdventureTime!-_-"
#define HOSTNAME "ESP-CO2-logger"

#define SSID_LENGTH     16
#define SSID_PW_LENGTH  32
#define HOSTNAME_LENGTH 16

// #define WEB_ENABLE

// I2C
// View header $(PROJECT_ROOT)/driver/driver/i2c_master.h

// Status LED
#define STATUS_LED_ENABLE
// #define STATUS_LED_TIME   2000
#define STATUS_LED_TIME_ON  4
#define STATUS_LED_TIME_OFF 30000

// Sensors
#define SCD30_READ_INTERVAL   3000
#define ZMOD_READ_INTERVAL    1990
#define SERVER_WRITE_INTERVAL 10000

// #define PRINT_ON_MEASURE_ENABLE

// Network
#define SNTP_SERVERNAME_0 "0.es.pool.ntp.org"
#define SNTP_SERVERNAME_1 "1.es.pool.ntp.org"
#define SNTP_TIMEZONE     1

#define INFLUX_URL ""

// Other
#define USE_OPTIMIZE_PRINTF
// #define MEMLEAK_DEBUG
