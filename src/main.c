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
#include <espconn.h>
#include <sntp.h>
#include <user_interface.h>

#include "extra_time.h"
#include "user_config.h"
#include "fast_gpio.h"
#include "uc_init.h"
#include "sensors.h"

#include "f2c/f2c.h"

#include "zmod4xxx/iaq_2nd_gen.h"
#include "zmod4xxx/zmod4xxx_types.h"

#include "simple_http/simple_http.h"


static zmod4xxx_dev_t zmod_dev;
static iaq_2nd_gen_handle_t iaq_handle;
static iaq_2nd_gen_results_t iaq_results;

static scd30_result_t scd30_result;

static uint8_t zmod4410_data_valid;
static uint8_t scd30_data_valid;

static volatile os_timer_t timer_blink;
static volatile os_timer_t timer_scd30;
static volatile os_timer_t timer_zmod;

static volatile os_timer_t timer_logger;

static struct espconn web_conn;


char* ICACHE_FLASH_ATTR
web_view(simple_http_server_request_info_t* p_data, size_t* p_data_size, uint16_t* p_response_code, http_content_type_t* p_content_type) {
    char* temp_data;
    char* data;
    char* f2c_str;

    /* Check if there is a GET request */
    if (os_strcmp(p_data->request_method, "GET") != 0) {
        *p_response_code = 403;
        *p_content_type = HTTP_CONTENT_TYPE_TEXT_HTML;

        data = (char*)os_malloc(sizeof(char) * 14);
        os_strcpy(data, "Nothing here!");
        *p_data_size = os_strlen(data);
    } else {
        data = (char*)os_malloc(sizeof(char) * ((F2C_CHAR_BUFF_SIZE + 6 + 4) * 5 + 4 + 2 + 2 + 4 + 10));
        temp_data = (char*)os_zalloc(sizeof(char) * F2C_CHAR_BUFF_SIZE);

        *p_response_code = 200;
        *p_content_type = HTTP_CONTENT_TYPE_TEXT_HTML;

        os_strcpy(data, "");
        if (scd30_data_valid) {
            os_strcat(data, "CO2: ");
            f2c_str = f2c(*((real32_t*) &scd30_result.co2), temp_data);
            os_strcat(data, f2c_str);
            os_strcat(data, " ppm<br>");

            os_strcat(data, "TEMP: ");
            f2c_str = f2c(*((real32_t*) &scd30_result.temp), temp_data);
            os_strcat(data, f2c_str);
            os_strcat(data, " C<br>");

            os_strcat(data, " RH: ");
            f2c_str = f2c(*((real32_t*) &scd30_result.rh), temp_data);
            os_strcat(data, f2c_str);
            os_strcat(data, " %<br>");
        } else {
            os_strcat(data, "SCD30 data not ready!<br>");
        }

        if (zmod4410_data_valid) {
            os_strcat(data, "eCO2: ");
            f2c_str = f2c(*((real32_t*) &iaq_results.eco2), temp_data);
            os_strcat(data, f2c_str);
            os_strcat(data, " ppm<br>");

            os_strcat(data, "IAQ: ");
            f2c_str = f2c(*((real32_t*) &iaq_results.iaq), temp_data);
            os_strcat(data, f2c_str);
        } else {
            os_strcat(data, "ZMOD4410 data not ready!");
        }

        os_free(temp_data);
        *p_data_size = os_strlen(data);
    }

    return data;
}

static void ICACHE_FLASH_ATTR
timer_send_data(void* args) {
    char* http_data_buff = (char*)os_malloc(sizeof(char) * (F2C_CHAR_BUFF_SIZE * 7 + 15));
    char* value_temp = (char*)os_malloc(sizeof(char) * F2C_CHAR_BUFF_SIZE);
    char* f2c_str;

    size_t print_len = 0;

    uint8_t send_en = 0;

    os_timer_disarm((os_timer_t*) &timer_logger);

    if (zmod4410_data_valid) {
        send_en = 1;

        print_len += os_sprintf(http_data_buff + print_len, "zmod ");

        f2c_str = f2c(*((real32_t*) &iaq_results.eco2), value_temp);
        print_len += os_sprintf(http_data_buff + print_len, "eco2=%s,", f2c_str);

        f2c_str = f2c(*((real32_t*) &iaq_results.etoh), value_temp);
        print_len += os_sprintf(http_data_buff + print_len, "etoh=%s,", f2c_str);

        f2c_str = f2c(*((real32_t*) &iaq_results.iaq), value_temp);
        print_len += os_sprintf(http_data_buff + print_len, "iaq=%s,", f2c_str);

        f2c_str = f2c(*((real32_t*) &iaq_results.tvoc), value_temp);
        print_len += os_sprintf(http_data_buff + print_len, "tvoc=%s\n", f2c_str);
    }

    if (scd30_data_valid) {
        send_en = 1;

        print_len += os_sprintf(http_data_buff + print_len, "scd30 ");

        f2c_str = f2c(*((real32_t*) &scd30_result.temp), value_temp);
        print_len += os_sprintf(http_data_buff + print_len, "temp=%s,", f2c_str);

        f2c_str = f2c(*((real32_t*) &scd30_result.co2), value_temp);
        print_len += os_sprintf(http_data_buff + print_len, "co2=%s,", f2c_str);

        f2c_str = f2c(*((real32_t*) &scd30_result.rh), value_temp);
        print_len += os_sprintf(http_data_buff + print_len, "rh=%s", f2c_str);
    }

    if (send_en) {
        simple_http_request(INFLUX_URL, http_data_buff, NULL, "POST", NULL);
        os_delay_us(2000);
    }

    os_free(http_data_buff);
    os_free(value_temp);

    os_timer_arm((os_timer_t*)&timer_logger, SERVER_WRITE_INTERVAL, 1);
}

static void ICACHE_FLASH_ATTR
print_scd30_results() {
    char* txt_buff = os_zalloc(sizeof(char) * F2C_CHAR_BUFF_SIZE);
    char* txt_f2c;

    txt_f2c = f2c(*((real32_t*) &scd30_result.co2), txt_buff);
    os_printf("CO2: %s ppm\n", txt_f2c);

    txt_f2c = f2c(*(real32_t*) &scd30_result.temp, txt_buff);
    os_printf("T:   %s C\n", txt_f2c);

    txt_f2c = f2c(*(real32_t*) &scd30_result.rh, txt_buff);
    os_printf("RH:  %s %%\n\n", txt_f2c);

    os_free(txt_buff);
}

static void ICACHE_FLASH_ATTR
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

    system_soft_wdt_feed();

    scd30_data_valid = 0;
    result = read_scd30(&scd30_result);

    if (result == SENSOR_READ_VALID) {
        // print_scd30_results();
        scd30_data_valid = 1;
    }
}

void ICACHE_FLASH_ATTR
timer_func_zmod(void* args) {
    sensor_status_t result;

    os_timer_disarm((os_timer_t*)&timer_zmod);
    system_soft_wdt_feed();

    zmod4410_data_valid = 0;
    result = read_zmod(&zmod_dev, &iaq_handle, &iaq_results);

    if (result == SENSOR_READ_VALID) {
        // print_zmod_results();
        zmod4410_data_valid = 1;
    } else if (result == SENSOR_ZMOD_STABILIZATION) {
        // os_printf("ZMOD stabilization process!\n");
    } else {
        // os_printf("ZMOD critical error %d\n", result);
    }

    os_timer_arm((os_timer_t*)&timer_zmod, ZMOD_READ_INTERVAL, 1);
}

void ICACHE_FLASH_ATTR
user_pre_init() {}

void ICACHE_FLASH_ATTR
user_init() {
    status_t status;

    os_printf("** user_init **\n");

    status = uc_init_uart();
    status = uc_init_wifi();
    status = uc_init_gpio();
    status = uc_init_i2c();
    status = uc_init_sntp();
    status = uc_init_sensors(&zmod_dev, &iaq_handle);

    if (status != STA_OK) {
#ifdef DEBUG_PRINT_MODE
        os_printf("uc init failed, please restart!\n");
#endif
        for (;;) {
            system_soft_wdt_feed();
            os_delay_us(2000);
        }
    } else {
#ifdef DEBUG_PRINT_MODE
        os_printf("Init done!\n");
#endif

        read_zmod(&zmod_dev, &iaq_handle, &iaq_results);

        // Timers
        os_timer_setfn((os_timer_t*)&timer_blink, (os_timer_func_t *)timer_func_blink, NULL);
#ifdef STATUS_LED_ENABLE
        os_timer_arm((os_timer_t*)&timer_blink, STATUS_LED_TIME, 1);
#else
        GPIO2_L;
#endif /* STATUS_LED_ENABLE */

        os_timer_setfn((os_timer_t*)&timer_scd30, (os_timer_func_t *)timer_func_scd30, NULL);
        os_timer_arm((os_timer_t*)&timer_scd30, SCD30_READ_INTERVAL, 1);

        os_timer_setfn((os_timer_t*)&timer_zmod, (os_timer_func_t *)timer_func_zmod, NULL);
        os_timer_arm((os_timer_t*)&timer_zmod, ZMOD_READ_INTERVAL, 1);

        create_basic_http_server(&web_conn, 80, web_view);

        os_timer_setfn((os_timer_t*)&timer_logger, (os_timer_func_t *)timer_send_data, NULL);
        os_timer_arm((os_timer_t*)&timer_logger, SERVER_WRITE_INTERVAL, 1);
    }
}
