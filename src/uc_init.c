/**
 * \file uc_init.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief uC init stuff. Source file.
 * \version 0.1
 * \date 2020-12-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */


#include "uc_init.h"
#include "driver/uart.h"
#include "simple_i2c.h"


status_t
uc_init_uart() {
    uart_div_modify(UART0, UART_CLK_FREQ / BITRATE);
    os_delay_us(1000);

    os_printf("UART init - Ok\n");
    return STA_OK;
}

status_t
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

    memcpy(&sta_cfg.ssid, ssid, SSID_LENGTH);
    memcpy(&sta_cfg.password, ssid_pw, SSID_PW_LENGTH);

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

    os_printf("WiFi init - Ok\n");
    return STA_OK;
}

status_t
uc_init_i2c() {
    uint8_t result;
    I2C_INIT(&result);

    os_printf("I2C init - Ok\n");
    return STA_OK;
}
