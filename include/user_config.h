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
#define ZMOD_READ_INTERVAL    5475
#define CCS_READ_INTERVAL     10101
#define SERVER_WRITE_INTERVAL 10201

// 111  * 6 ≈ 666s  ≈ 11min
// 1111 * 6 ≈ 6666s ≈ 1111min ≈ 1h 50min
#define ZMOD_TEST_RESET_COUNT_ON  111   /* ZMOD measures before reseting ZMOD variables */
#define ZMOD_TEST_RESET_COUNT_OFF 1111  /* Ignored ZMOD measures */

#define ZMOD_TEST_HALT_COUNT_ON   111   /* ZMOD measures before halt */
#define ZMOD_TEST_HALT_COUNT_OFF  1111  /* Ignored ZMOD measures */

// #define PRINT_ON_MEASURE_ENABLE

// Network
#define SNTP_SERVERNAME_0 "0.es.pool.ntp.org"
#define SNTP_SERVERNAME_1 "1.es.pool.ntp.org"
#define SNTP_TIMEZONE     1

// InfluxDB 2
#define INFLUX_URL         "http://<db_url>/api/v2/write?org=<org>>&bucket=<bucket_name>&precision=s"
#define INFLUX_TOKEN       ""
#define INFLUX_AUTH_HEADER "Authorization: Token "INFLUX_TOKEN

// Other
#define USE_OPTIMIZE_PRINTF
// #define MEMLEAK_DEBUG
