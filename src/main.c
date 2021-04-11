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

#include "zmod4xxx/iaq_1st_gen.h"
#include "zmod4xxx/zmod4xxx_types.h"

#include "simple_http/simple_http.h"

#include "ccs811/ccs811.h"
#include "ccs811/ccs811_defs.h"


static zmod4xxx_dev_t zmod_dev;
static uint8_t zmod_adc_result[SENSORS_ADC_RESULT_SIZE];

static iaq_1st_gen_handle_t  iaq_handle;
static iaq_1st_gen_results_t iaq_results;

static iaq_1st_gen_handle_t  iaq_handle_test_reset;
static iaq_1st_gen_results_t iaq_results_test_reset;

static iaq_1st_gen_handle_t  iaq_handle_test_halt;
static iaq_1st_gen_results_t iaq_results_test_halt;

uint16_t zmod_reset_counter, zmod_halt_counter;

static scd30_result_t scd30_result;

static ccs811_data_t ccs_data;
static ccs811_dev_t ccs_dev;

static uint8_t zmod4410_data_valid, zmod4410_data_valid_reset, zmod4410_data_valid_halt;
static uint8_t ccs811_data_valid;
static uint8_t scd30_data_valid;

static volatile os_timer_t timer_blink;
static volatile os_timer_t timer_scd30;
static volatile os_timer_t timer_zmod;
static volatile os_timer_t timer_zmod_reset;
static volatile os_timer_t timer_zmod_halt;
static volatile os_timer_t timer_ccs;

static volatile os_timer_t timer_logger;

#ifdef WEB_ENABLE
static struct espconn web_conn;
#endif


#ifdef WEB_ENABLE
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
#endif /* WEB_ENABLE */

static size_t ICACHE_FLASH_ATTR
zmod_sprintf(char* name, iaq_1st_gen_results_t* zmod_results, size_t print_len, char* http_data_buff, char* value_temp) {
    char* f2c_str;
    float zmod_rcda, zmod_rmox, zmod_iaq, zmod_tvoc, zmod_etoh, zmod_eco2;

    size_t int_print_len = print_len;

    zmod_eco2 = zmod_results->eco2;
    zmod_etoh = zmod_results->etoh;
    zmod_rcda = zmod_results->rcda;
    zmod_iaq  = zmod_results->iaq;
    zmod_tvoc = zmod_results->tvoc;
    zmod_rmox = zmod_results->rmox;

    int_print_len += os_sprintf(http_data_buff + int_print_len, "%s ", name);

    f2c_str = f2c(zmod_eco2, value_temp);
    int_print_len += os_sprintf(http_data_buff + int_print_len, "eco2=%s,", f2c_str);

    f2c_str = f2c(zmod_etoh, value_temp);
    int_print_len += os_sprintf(http_data_buff + int_print_len, "etoh=%s,", f2c_str);

    f2c_str = f2c(zmod_rcda, value_temp);
    int_print_len += os_sprintf(http_data_buff + int_print_len, "rcda=%s,", f2c_str);

    f2c_str = f2c(zmod_iaq, value_temp);
    int_print_len += os_sprintf(http_data_buff + int_print_len, "iaq=%s,", f2c_str);

    f2c_str = f2c(zmod_tvoc, value_temp);
    int_print_len += os_sprintf(http_data_buff + int_print_len, "tvoc=%s,", f2c_str);

    f2c_str = f2c(zmod_rmox, value_temp);
    int_print_len += os_sprintf(http_data_buff + int_print_len, "rmox=%s", f2c_str);

    int_print_len += os_sprintf(http_data_buff + int_print_len, "\n");

    return int_print_len;
}

static void ICACHE_FLASH_ATTR
timer_send_data(void* args) {
    char* http_data_buff = (char*)os_malloc(sizeof(char) * (F2C_CHAR_BUFF_SIZE * 25 + 55*4));
    char* value_temp = (char*)os_malloc(sizeof(char) * F2C_CHAR_BUFF_SIZE);
    char* f2c_str;

    uint32_t scd30_temp, scd30_co2, scd30_rh;
    uint16_t ccs_eco2, ccs_tvocs, ccs_raw;

    size_t print_len = 0;

    uint8_t send_en = 0;

    simple_http_status_t http_status;

    if (value_temp == NULL || http_data_buff == NULL) {
        return;
    }

    // os_timer_disarm((os_timer_t*) &timer_logger);

    if (zmod4410_data_valid) {
        print_len = zmod_sprintf("zmod", &iaq_results, print_len, http_data_buff, value_temp);
        send_en = 1;
    }

    if (zmod4410_data_valid_reset) {
        print_len = zmod_sprintf("zmod_reset", &iaq_results_test_reset, print_len, http_data_buff, value_temp);
        send_en = 1;
    }

    if (zmod4410_data_valid_halt) {
        print_len = zmod_sprintf("zmod_halt", &iaq_results_test_halt, print_len, http_data_buff, value_temp);
        send_en = 1;
    }

    if (scd30_data_valid) {
        scd30_temp = scd30_result.temp;
        scd30_co2  = scd30_result.co2;
        scd30_rh   = scd30_result.rh;
        send_en    = 1;

        print_len += os_sprintf(http_data_buff + print_len, "scd30 ");

        f2c_str = f2c(*((real32_t*)&scd30_temp), value_temp);
        print_len += os_sprintf(http_data_buff + print_len, "temp=%s,", f2c_str);

        f2c_str = f2c(*((real32_t*)&scd30_co2), value_temp);
        print_len += os_sprintf(http_data_buff + print_len, "co2=%s,", f2c_str);

        f2c_str = f2c(*((real32_t*)&scd30_rh), value_temp);
        print_len += os_sprintf(http_data_buff + print_len, "rh=%s", f2c_str);

        print_len += os_sprintf(http_data_buff + print_len, "\n");
    }

    if (ccs811_data_valid) {
        ccs_eco2  = ccs_data.eco2;
        ccs_tvocs = ccs_data.tvoc;
        ccs_raw   = ccs_data.raw_data;
        send_en   = 1;

        print_len += os_sprintf(http_data_buff + print_len, "ccs811 ");
        print_len += os_sprintf(http_data_buff + print_len, "eco2=%u,", ccs_eco2);
        print_len += os_sprintf(http_data_buff + print_len, "tvoc=%u,", ccs_tvocs);
        print_len += os_sprintf(http_data_buff + print_len, "current=%u,", CCS811_RAW_DATA_CURRENT(ccs_raw));
        print_len += os_sprintf(http_data_buff + print_len, "adc=%u", CCS811_RAW_DATA_ADC(ccs_raw));
        print_len += os_sprintf(http_data_buff + print_len, "\n");
    }

    // os_printf("Free dyn mem = %lu\n", system_get_free_heap_size());
    if (send_en) {
        http_status = simple_http_request(INFLUX_URL, http_data_buff, INFLUX_AUTH_HEADER"\r\n", "POST", NULL);
        os_printf("Sending:\n%s\nResult = %d\n\n", http_data_buff, http_status);

        os_delay_us(2000);
        system_soft_wdt_feed();
    }

    os_free(http_data_buff);
    os_free(value_temp);

    os_timer_arm((os_timer_t*)&timer_logger, SERVER_WRITE_INTERVAL, 0);
}

#ifdef PRINT_ON_MEASURE_ENABLE
static void ICACHE_FLASH_ATTR
print_scd30_results() {
    char* txt_buff = os_zalloc(sizeof(char) * F2C_CHAR_BUFF_SIZE);
    char* txt_f2c;

    os_printf("SCD30:\n");

    txt_f2c = f2c(*((real32_t*) &scd30_result.co2), txt_buff);
    os_printf("\tCO2: %s ppm\n", txt_f2c);

    txt_f2c = f2c(*(real32_t*) &scd30_result.temp, txt_buff);
    os_printf("\tT:   %s C\n", txt_f2c);

    txt_f2c = f2c(*(real32_t*) &scd30_result.rh, txt_buff);
    os_printf("\tRH:  %s %%\n\n", txt_f2c);

    os_free(txt_buff);
}

static void ICACHE_FLASH_ATTR
print_zmod_param_results(iaq_1st_gen_results_t* iaq_results) {
    char* txt_buff = os_zalloc(sizeof(char) * F2C_CHAR_BUFF_SIZE);
    char* txt_f2c;

    os_printf("ZMOD4410:\n");

    txt_f2c = f2c((real32_t) iaq_results->eco2, txt_buff);
    os_printf("\teCO2: %s ppm\n", txt_f2c);

    txt_f2c = f2c((real32_t) iaq_results->etoh, txt_buff);
    os_printf("\teTOH: %s ppm\n", txt_f2c);

    txt_f2c = f2c((real32_t) iaq_results->iaq, txt_buff);
    os_printf("\tIAQ: %s\n", txt_f2c);

    txt_f2c = f2c((real32_t) iaq_results->tvoc, txt_buff);
    os_printf("\tTVOCs: %s mg/m^3\n", txt_f2c);

    txt_f2c = f2c((real32_t) iaq_results->rcda, txt_buff);
    os_printf("\tRCDA: %s ohm\n", txt_f2c);

    txt_f2c = f2c((real32_t) iaq_results->rmox, txt_buff);
    os_printf("\tRMOX: %s ohm\n\n", txt_f2c);

    os_free(txt_buff);
}

static void ICACHE_FLASH_ATTR
print_zmod_results() {
    print_zmod_param_results(&iaq_results);
}

static void ICACHE_FLASH_ATTR
print_ccs_results() {
    os_printf("CCS811:\n");
    os_printf("\teCO2: %uppm [0x%04x]\n", ccs_data.eco2, ccs_data.eco2);
    os_printf("\tTVOCs: %uppb [0x%04x]\n", ccs_data.tvoc, ccs_data.tvoc);
    os_printf("\tstatus: 0x%02x\n", ccs_data.status);
    os_printf("\terror_id: 0x%02x\n", ccs_data.error_id);
    os_printf("\traw_data: current=%uuA ADC=%u [0x%04x]\n\n",
        CCS811_RAW_DATA_CURRENT(ccs_data.raw_data), CCS811_RAW_DATA_ADC(ccs_data.raw_data), ccs_data.raw_data);
}
#endif /* PRINT_ON_MEASURE_ENABLE */

void ICACHE_FLASH_ATTR
timer_func_blink(void* args) {
    os_timer_disarm((os_timer_t*)&timer_blink);

    if (GPIO2_IN) {
        GPIO2_L;
        os_timer_arm((os_timer_t*)&timer_blink, STATUS_LED_TIME_ON, 0);
    } else {
        GPIO2_H;
        os_timer_arm((os_timer_t*)&timer_blink, STATUS_LED_TIME_OFF, 0);
    }

    // GPIO2_IN ? GPIO2_L : GPIO2_H;
}

void ICACHE_FLASH_ATTR
timer_func_scd30(void* args) {
    sensor_status_t result;

    system_soft_wdt_feed();

    scd30_data_valid = 0;
    result = read_scd30(&scd30_result);

    if (result == SENSOR_READ_VALID) {
#ifdef PRINT_ON_MEASURE_ENABLE
        print_scd30_results();
#endif
        scd30_data_valid = 1;
    }
}

void ICACHE_FLASH_ATTR
timer_func_zmod_reset_end(void* args) {
    os_timer_disarm((os_timer_t*)&timer_zmod_reset);
    memset(&iaq_handle_test_reset, 0, sizeof(iaq_handle_test_reset));
    uc_init_zmod_test(&zmod_dev, &iaq_handle_test_reset);
    zmod_reset_counter = 0;
}

void ICACHE_FLASH_ATTR
timer_func_zmod_halt_end(void* args) {
    os_timer_disarm((os_timer_t*)&timer_zmod_halt);
    zmod_halt_counter = 0;
}

void ICACHE_FLASH_ATTR
timer_func_zmod(void* args) {
    sensor_status_t result;

    os_timer_disarm((os_timer_t*)&timer_zmod);
    system_soft_wdt_feed();

    // LP continuous mode
    zmod4410_data_valid = 0;
    result = read_zmod(&zmod_dev, &iaq_handle, &iaq_results, zmod_adc_result);

    if (result == SENSOR_READ_VALID) {
#ifdef PRINT_ON_MEASURE_ENABLE
        os_printf("## iaq_results ##\n");
        print_zmod_results();
#endif
        zmod4410_data_valid = 1;
    } else if (result == SENSOR_ZMOD_STABILIZATION) {
#ifdef PRINT_ON_MEASURE_ENABLE
        os_printf("ZMOD stabilization process!\n");
#endif
    } else {
#ifdef PRINT_ON_MEASURE_ENABLE
        os_printf("ZMOD critical error %d\n", result);
#endif
    }

    // LP mode with halt and reset
    zmod4410_data_valid_reset = 0;
    if (zmod_reset_counter == ZMOD_TEST_RESET_COUNT) {
        os_timer_arm((os_timer_t*)&timer_zmod_reset, ZMOD_TEST_RESET_DELAY, 0);
        zmod_reset_counter += 1;
    } else if (zmod_reset_counter < ZMOD_TEST_RESET_COUNT) {
        result = calc_zmod_result(&zmod_dev, &iaq_handle_test_reset, &iaq_results_test_reset, zmod_adc_result);

        if (result == SENSOR_READ_VALID) {
#ifdef PRINT_ON_MEASURE_ENABLE
            os_printf("## iaq_results_test_reset ##\n");
            print_zmod_param_results(&iaq_results_test_reset);
#endif
            zmod_reset_counter += 1;
            zmod4410_data_valid_reset = 1;
        } else if (result == SENSOR_ZMOD_STABILIZATION) {
#ifdef PRINT_ON_MEASURE_ENABLE
            os_printf("ZMOD [w/ reset] stabilization process!\n");
#endif
        } else {
#ifdef PRINT_ON_MEASURE_ENABLE
            os_printf("ZMOD [w/ reset] critical error %d\n", result);
#endif
        }
    } else {
#ifdef PRINT_ON_MEASURE_ENABLE
        os_printf("ZMOD [w/ reset] delay");
#endif
    }

    // LP mode with halt
    zmod4410_data_valid_halt = 0;
    if (zmod_halt_counter == ZMOD_TEST_HALT_COUNT) {
        os_timer_arm((os_timer_t*)&timer_zmod_halt, ZMOD_TEST_HALT_DELAY, 0);
        zmod_halt_counter += 1;
    } else if (zmod_halt_counter < ZMOD_TEST_HALT_COUNT) {
        result = calc_zmod_result(&zmod_dev, &iaq_handle_test_halt, &iaq_results_test_halt, zmod_adc_result);

        if (result == SENSOR_READ_VALID) {
#ifdef PRINT_ON_MEASURE_ENABLE
            os_printf("## iaq_results_test_halt ##\n");
            print_zmod_param_results(&iaq_results_test_halt);
#endif
            zmod_halt_counter += 1;
            zmod4410_data_valid_halt = 1;
        } else if (result == SENSOR_ZMOD_STABILIZATION) {
#ifdef PRINT_ON_MEASURE_ENABLE
            os_printf("ZMOD [w/ halt] stabilization process!\n");
#endif
        } else {
#ifdef PRINT_ON_MEASURE_ENABLE
            os_printf("ZMOD [w/ halt] critical error %d\n", result);
#endif
        }
    } else {
#ifdef PRINT_ON_MEASURE_ENABLE
        os_printf("ZMOD [w/ halt] delay");
#endif
    }

    os_timer_arm((os_timer_t*)&timer_zmod, ZMOD_READ_INTERVAL, 0);
}

void ICACHE_FLASH_ATTR
timer_func_ccs(void* args) {
    sensor_status_t result;

    os_timer_disarm((os_timer_t*)&timer_ccs);
    system_soft_wdt_feed();

    ccs811_data_valid = 0;
    result = read_ccs811(&ccs_dev, &ccs_data);

    if (result == SENSOR_READ_VALID) {
#ifdef PRINT_ON_MEASURE_ENABLE
        print_ccs_results();
#endif
        ccs811_data_valid = 1;
    } else if (result == SENSOR_NOT_READY) {
#ifdef PRINT_ON_MEASURE_ENABLE
        os_printf("CCS811 data not ready!\n");
#endif
    } else {
#ifdef PRINT_ON_MEASURE_ENABLE
        os_printf("CCS811 critical error %d\n", result);
#endif
    }

    os_timer_arm((os_timer_t*)&timer_ccs, CCS_READ_INTERVAL, 0);
}

void ICACHE_FLASH_ATTR
user_pre_init() {}

void ICACHE_FLASH_ATTR
user_init() {
    status_t status;

#ifdef DEBUG_PRINT_MODE
    os_printf("** user_init **\n");
#endif

    status = uc_init_uart();
    status = uc_init_wifi();
    status = uc_init_gpio();
    status = uc_init_i2c();
    status = uc_init_sntp();
    status = uc_init_sensors(&zmod_dev, &iaq_handle, &ccs_dev);
    status = uc_init_zmod_test(&zmod_dev, &iaq_handle_test_reset);
    status = uc_init_zmod_test(&zmod_dev, &iaq_handle_test_halt);

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
        zmod_reset_counter = 0;
        zmod_halt_counter = 0;
        read_zmod(&zmod_dev, &iaq_handle, &iaq_results, zmod_adc_result);

        // Timers
        os_timer_setfn((os_timer_t*)&timer_blink, (os_timer_func_t *)timer_func_blink, NULL);
#ifdef STATUS_LED_ENABLE
        os_timer_arm((os_timer_t*)&timer_blink, STATUS_LED_TIME_ON, 0);
#else
        GPIO2_H;
#endif /* STATUS_LED_ENABLE */

        os_timer_setfn((os_timer_t*)&timer_scd30, (os_timer_func_t *)timer_func_scd30, NULL);
        os_timer_arm((os_timer_t*)&timer_scd30, SCD30_READ_INTERVAL, 1);

        os_timer_setfn((os_timer_t*)&timer_zmod, (os_timer_func_t *)timer_func_zmod, NULL);
        os_timer_arm((os_timer_t*)&timer_zmod, ZMOD_READ_INTERVAL, 0);

        os_timer_setfn((os_timer_t*)&timer_zmod_reset, (os_timer_func_t *)timer_func_zmod_reset_end, NULL);

        os_timer_setfn((os_timer_t*)&timer_zmod_halt, (os_timer_func_t *)timer_func_zmod_halt_end, NULL);

        os_timer_setfn((os_timer_t*)&timer_ccs, (os_timer_func_t *)timer_func_ccs, NULL);
        os_timer_arm((os_timer_t*)&timer_ccs, CCS_READ_INTERVAL, 0);

        os_timer_setfn((os_timer_t*)&timer_logger, (os_timer_func_t *)timer_send_data, NULL);
        os_timer_arm((os_timer_t*)&timer_logger, SERVER_WRITE_INTERVAL, 0);

#ifdef WEB_ENABLE
        create_basic_http_server(&web_conn, 80, web_view);
#endif
    }
}
