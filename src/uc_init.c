/**
 * \file uc_init.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief uC init stuff. Source file.
 * \version 0.1
 * \date 2020-12-13
 */

#include <sntp.h>
#include <osapi.h>
#include <user_interface.h>

#include "uc_init.h"
#include "fast_gpio.h"
#include "simple_i2c.h"
#include "user_config.h"
#include "driver/uart.h"

#include "zmod4xxx/zmod4xxx.h"
#include "zmod4xxx/iaq_2nd_gen.h"
#include "zmod4xxx/zmod4xxx_hal.h"
#include "zmod4xxx/zmod4xxx_config.h"

#include "ccs811/ccs811.h"
#include "ccs811/ccs811_defs.h"
#include "ccs811/ccs811_hal.h"


status_t ICACHE_FLASH_ATTR
uc_init_uart() {
    uart_div_modify(UART0, UART_CLK_FREQ / BITRATE);
    os_delay_us(1000);

#ifdef DEBUG_PRINT_MODE
    os_printf("UART init - Ok\n");
#endif
    return STA_OK;
}

status_t ICACHE_FLASH_ATTR
uc_init_wifi() {
    station_config_t sta_cfg;
    uint8_t ssid[SSID_LENGTH] = SSID;
    uint8_t ssid_pw[SSID_PW_LENGTH] = SSID_PW;
    uint8_t hostname[HOSTNAME_LENGTH] = HOSTNAME;

    if(wifi_get_opmode() != STATION_MODE) {
        if (!wifi_set_opmode(STATION_MODE)) {
            return STA_ERR;
        }
	}	

    os_memcpy(&sta_cfg.ssid, ssid, SSID_LENGTH);
    os_memcpy(&sta_cfg.password, ssid_pw, SSID_PW_LENGTH);

    if (!wifi_station_set_config(&sta_cfg)) {
        return STA_ERR;
    }
    if (!wifi_station_set_hostname((char*)&hostname)) {
        return STA_ERR;
    }
    if (!wifi_station_connect()) {
        return STA_ERR;
    }
    os_delay_us(1000);

    wifi_set_sleep_type(MODEM_SLEEP_T);

#ifdef DEBUG_PRINT_MODE
    os_printf("WiFi init - Ok\n");
#endif
    return STA_OK;
}

status_t ICACHE_FLASH_ATTR
uc_init_gpio() {
    GPIO2_OUTPUT_SET;

#ifdef DEBUG_PRINT_MODE
    os_printf("GPIO init - Ok\n");
#endif
    return STA_OK;
}

status_t ICACHE_FLASH_ATTR
uc_init_i2c() {
    uint8_t result;
    I2C_INIT(&result);

#ifdef DEBUG_PRINT_MODE
    os_printf("I2C init - Ok\n");
#endif
    return STA_OK;
}

status_t ICACHE_FLASH_ATTR
uc_init_sensors(zmod4xxx_dev_t* zmod_dev, iaq_2nd_gen_handle_t* iaq_2nd_handle, ccs811_dev_t* ccs_dev) {
    int8_t zmod_result;
    uint8_t css_status;
    uint8_t* p_prod_data = (uint8_t*)os_malloc(sizeof(uint8_t) * ZMOD4410_PROD_DATA_LEN);

    ccs811_result_t css_result;

    // ZMOD4410
    zmod4xxx_init_hw(zmod_dev);
    zmod_dev->i2c_addr  = ZMOD4410_I2C_ADDR;
    zmod_dev->pid       = ZMOD4410_PID;
    zmod_dev->init_conf = &zmod_sensor_type[INIT];
    zmod_dev->meas_conf = &zmod_sensor_type[MEASUREMENT];
    zmod_dev->prod_data = p_prod_data;

    zmod_result = zmod4xxx_read_sensor_info(zmod_dev);
    if (zmod_result) {
#ifdef DEBUG_PRINT_MODE
        os_printf("ZMOD4410 HW initialize error [%d]!\n", zmod_result);
#endif
        return STA_ERR;
    }

    zmod_result = zmod4xxx_prepare_sensor(zmod_dev);
    if (zmod_result) {
#ifdef DEBUG_PRINT_MODE
        os_printf("ZMOD4410 sensor preparation error [%d]!\n", zmod_result);
#endif
        return STA_ERR;
    }

    zmod_result = init_iaq_2nd_gen(iaq_2nd_handle);
    if (zmod_result) {
#ifdef DEBUG_PRINT_MODE
        os_printf("ZMOD4410 algorithm initializing error [%d]!\n", zmod_result);
#endif
        return STA_ERR;
    }

    zmod_result = zmod4xxx_start_measurement(zmod_dev);
    if (zmod_result) {
#ifdef DEBUG_PRINT_MODE
        os_printf("ZMOD4410 measurement starting error [%d]!\n", zmod_result);
#endif
        return STA_ERR;
    }

    // CCS811
    ccs_dev->i2c_addr = CCS811_I2C_ADDR_LOW;

    css_result = ccs811_init_hw(ccs_dev);
    if (css_result != CCS811_OK) {
#ifdef DEBUG_PRINT_MODE
        os_printf("CCS811 HW init error [%d]!\n", css_result);
#endif
        return STA_ERR;
    }

    css_result = ccs811_reset(ccs_dev);
    if (css_result != CCS811_OK) {
#ifdef DEBUG_PRINT_MODE
        os_printf("CCS811 reset error [%d]!\n", css_result);
#endif
        return STA_ERR;
    }

    css_result = ccs811_get_sensor_info(ccs_dev);
    if (css_result != CCS811_OK) {
#ifdef DEBUG_PRINT_MODE
        os_printf("CCS811 error loading sensor info [%d]!\n", css_result);
#endif
        return STA_ERR;
    }

    css_result = ccs811_get_status(ccs_dev, &css_status);
    if (css_result != CCS811_OK) {
#ifdef DEBUG_PRINT_MODE
        os_printf("CCS811 error getting sensor status - A [%d]!\n", css_result);
#endif
        return STA_ERR;
    } else if (!CCS811_STATUS_APP_VALID(css_status)) {
#ifdef DEBUG_PRINT_MODE
        os_printf("CCS811 error app not valid!\n");
#endif
        return STA_ERR;
    }

    css_result = ccs811_app_start(ccs_dev);
    if (css_result != CCS811_OK) {
#ifdef DEBUG_PRINT_MODE
        os_printf("CCS811 error starting APP [%d]!\n", css_result);
#endif
        return STA_ERR;
    }

    css_result = ccs811_get_status(ccs_dev, &css_status);
    if (css_result != CCS811_OK) {
#ifdef DEBUG_PRINT_MODE
        os_printf("CCS811 error getting sensor status - B [%d]!\n", css_result);
#endif
        return STA_ERR;
    } else if (CCS811_STATUS_FW_MODE(css_status) != 1) {
#ifdef DEBUG_PRINT_MODE
        os_printf("CCS811 APP FW not loaded!\n");
#endif
        return STA_ERR;
    }

    css_result = ccs811_set_meas_mode(ccs_dev, CCS811_DRIVE_MODE_10S, CCS811_INT_DISABLE, CCS811_INT_THRESH_OFF);
    if (css_result != CCS811_OK) {
#ifdef DEBUG_PRINT_MODE
        os_printf("CCS811 error setting measurement mode [%d]!\n", css_result);
#endif
        return STA_ERR;
    }

#ifdef DEBUG_PRINT_MODE
    os_printf("Sensors init - Ok\n");
#endif
    return STA_OK;
}

status_t ICACHE_FLASH_ATTR
uc_init_sntp() {
#ifdef SNTP_SERVERNAME_0
    sntp_setservername(0, SNTP_SERVERNAME_0);
#endif

#ifdef SNTP_SERVERNAME_1
    sntp_setservername(1, SNTP_SERVERNAME_1);
#endif

#ifdef SNTP_SERVERNAME_2
    sntp_setservername(2, SNTP_SERVERNAME_2);
#endif

    sntp_set_timezone(SNTP_TIMEZONE);
    sntp_init();

#ifdef DEBUG_PRINT_MODE
    os_printf("Network init - Ok\n");
#endif
    return STA_OK;
}
