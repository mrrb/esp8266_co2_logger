/**
 * \file uc_init.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief uC init stuff. Source file.
 * \version 0.1
 * \date 2020-12-13
 */

#include <osapi.h>

#include "uc_init.h"
#include "fast_gpio.h"
#include "simple_i2c.h"
#include "user_config.h"
#include "driver/uart.h"

#include "zmod4xxx/zmod4xxx.h"
#include "zmod4xxx/iaq_2nd_gen.h"
#include "zmod4xxx/zmod4xxx_hal.h"
#include "zmod4xxx/zmod4xxx_config.h"


status_t
uc_init_uart() {
    uart_div_modify(UART0, UART_CLK_FREQ / BITRATE);
    os_delay_us(1000);

#ifdef DEBUG_MODE
    os_printf("UART init - Ok\n");
#endif
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

#ifdef DEBUG_MODE
    os_printf("WiFi init - Ok\n");
#endif
    return STA_OK;
}

status_t
uc_init_gpio() {
    GPIO2_OUTPUT_SET;

#ifdef DEBUG_MODE
    os_printf("GPIO init - Ok\n");
#endif
    return STA_OK;
}

status_t
uc_init_i2c() {
    uint8_t result;
    I2C_INIT(&result);

#ifdef DEBUG_MODE
    os_printf("I2C init - Ok\n");
#endif
    return STA_OK;
}

status_t
uc_init_sensors(zmod4xxx_dev_t* zmod_dev, iaq_2nd_gen_handle_t* iaq_2nd_handle) {
    int8_t zmod_result;
    uint8_t prod_data[ZMOD4410_PROD_DATA_LEN];

    zmod4xxx_init_hw(zmod_dev);
    zmod_dev->i2c_addr  = ZMOD4410_I2C_ADDR;
    zmod_dev->pid       = ZMOD4410_PID;
    zmod_dev->init_conf = &zmod_sensor_type[INIT];
    zmod_dev->meas_conf = &zmod_sensor_type[MEASUREMENT];
    zmod_dev->prod_data = prod_data;

    zmod_result = zmod4xxx_read_sensor_info(zmod_dev);
    if (zmod_result) {
#ifdef DEBUG_MODE
        os_printf("ZMOD4410 HW initialize error [%d]!\n", zmod_result);
#endif
        return STA_ERR;
    }

    zmod_result = zmod4xxx_prepare_sensor(zmod_dev);
    if (zmod_result) {
#ifdef DEBUG_MODE
        os_printf("ZMOD4410 sensor preparation error [%d]!\n", zmod_result);
#endif
        return STA_ERR;
    }

    zmod_result = init_iaq_2nd_gen(iaq_2nd_handle);
    if (zmod_result) {
#ifdef DEBUG_MODE
        os_printf("ZMOD4410 algorithm initializing error [%d]!\n", zmod_result);
#endif
        return STA_ERR;
    }

    zmod_result = zmod4xxx_start_measurement(zmod_dev);
    if (zmod_result) {
#ifdef DEBUG_MODE
        os_printf("ZMOD4410 measurement starting error [%d]!\n", zmod_result);
#endif
        return STA_ERR;
    }

#ifdef DEBUG_MODE
    os_printf("Sensors init - Ok\n");
#endif
    return STA_OK;
}
