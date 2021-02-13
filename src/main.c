/**
 * \file main.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2020-12-13
 */

#include <osapi.h>
#include <c_types.h>
#include <inttypes.h>

#include "user_config.h"
#include "fast_gpio.h"
#include "uc_init.h"
#include "sensors.h"

#include "f2c/f2c.h"

#include "zmod4xxx/iaq_2nd_gen.h"
#include "zmod4xxx/zmod4xxx_types.h"


static zmod4xxx_dev_t zmod_dev;
static iaq_2nd_gen_handle_t iaq_handle;
static iaq_2nd_gen_results_t iaq_results;

static scd30_result_t scd30_result;

static uint8_t zmod4410_data_valid;
static uint8_t scd30_data_valid;

static volatile os_timer_t timer_blink;
static volatile os_timer_t timer_scd30;
static volatile os_timer_t timer_zmod;


void ICACHE_FLASH_ATTR
print_scd30_results() {
    char* txt_buff = os_zalloc(sizeof(char) * F2C_CHAR_BUFF_SIZE);
    char* txt_f2c;

    txt_f2c = f2c(*((real32_t*) &(scd30_result.co2)), txt_buff);
    os_printf("CO2: %s ppm\n", txt_f2c);

    txt_f2c = f2c(*(real32_t*) &scd30_result.temp, txt_buff);
    os_printf("T:   %s C\n", txt_f2c);

    txt_f2c = f2c(*(real32_t*) &scd30_result.rh, txt_buff);
    os_printf("RH:  %s %%\n\n", txt_f2c);

    os_free(txt_buff);
}

void ICACHE_FLASH_ATTR
print_zmod_results() {
    char* txt_buff = os_zalloc(sizeof(char) * F2C_CHAR_BUFF_SIZE);
    char* txt_f2c;

    txt_f2c = f2c((real32_t) iaq_results.eco2, txt_buff);
    os_printf("eCO2: %s ppm\n", txt_f2c);

    txt_f2c = f2c((real32_t) iaq_results.etoh, txt_buff);
    os_printf("eTOH: %s ppm\n", txt_f2c);

    txt_f2c = f2c((real32_t) iaq_results.iaq, txt_buff);
    os_printf("IAQ: %s\n", txt_f2c);

    txt_f2c = f2c((real32_t) iaq_results.tvoc, txt_buff);
    os_printf("TVOC: %s mg/m^3\n", txt_f2c);

    txt_f2c = f2c((real32_t) iaq_results.tvoc, txt_buff);
    os_printf("log_Rcda: %s logOhm\n\n", txt_f2c);

    os_free(txt_buff);
}

void ICACHE_FLASH_ATTR
timer_func_blink(void* args) {
    GPIO2_IN ? GPIO2_L : GPIO2_H;
}

void ICACHE_FLASH_ATTR
timer_func_scd30(void* args) {
    sensor_status_t result;
    result = read_scd30(&scd30_result);

    scd30_data_valid = 0;
    if (result == SENSOR_READ_VALID) {
        print_scd30_results();
        scd30_data_valid = 1;
    }
}

void ICACHE_FLASH_ATTR
timer_func_zmod(void* args) {
    sensor_status_t result;

    os_timer_disarm((os_timer_t*)&timer_zmod);
    result = read_zmod(&zmod_dev, &iaq_handle, &iaq_results);

    zmod4410_data_valid = 0;
    if (result == SENSOR_READ_VALID) {
        print_zmod_results();
        zmod4410_data_valid = 1;
    } else if (result == SENSOR_ZMOD_STABILIZATION) {
        os_printf("ZMOD stabilization process!\n");
    } else {
        os_printf("ZMOD critical error %d\n", result);
    }

    os_timer_arm((os_timer_t*)&timer_zmod, ZMOD_READ_INTERVAL, 1);
}

void ICACHE_FLASH_ATTR
user_pre_init() {}

void ICACHE_FLASH_ATTR
user_init() {
    status_t status;

    status = uc_init_uart();
    status = uc_init_wifi();
    status = uc_init_gpio();
    status = uc_init_i2c();
    status = uc_init_sensors(&zmod_dev, &iaq_handle);
    read_zmod(&zmod_dev, &iaq_handle, &iaq_results);

    if (status != STA_OK) {
#ifdef DEBUG_MODE
        os_printf("uc init failed, please restart!\n");
#endif
        for (;;) {
            os_delay_us(1000);
        }
    }

    // Timers
    os_timer_setfn((os_timer_t*)&timer_blink, (os_timer_func_t *)timer_func_blink, NULL);
#ifdef STATUS_LED_ENABLE
    os_timer_arm((os_timer_t*)&timer_blink, STATUS_LED_TIME, 1);
#endif

    os_timer_setfn((os_timer_t*)&timer_scd30, (os_timer_func_t *)timer_func_scd30, NULL);
    os_timer_arm((os_timer_t*)&timer_scd30, SCD30_READ_INTERVAL, 1);

    os_timer_setfn((os_timer_t*)&timer_zmod, (os_timer_func_t *)timer_func_zmod, NULL);
    os_timer_arm((os_timer_t*)&timer_zmod, ZMOD_READ_INTERVAL, 1);
}
