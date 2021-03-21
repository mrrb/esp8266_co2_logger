/**
 * \file zmod4xxx_config.h
 * \author Renesas Electronics Corporation
 * \brief Renesas ZMOD4410 IAQ 1st gen provided config
 * \version 2.1.2
 */

#ifndef ZMOD4XXX_CONFIG_H
#define ZMOD4XXX_CONFIG_H

#include <c_types.h>
#include "zmod4xxx_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define INIT       0
#define CONTINUOUS 1
#define LOW_POWER  2

#define ZMOD_CONF_TYPE LOW_POWER

#if defined ZMOD_CONF_TYPE && ZMOD_CONF_TYPE == CONTINUOUS
#define ZMOD4410_SAMPLE_PERIOD 2
#elif defined ZMOD_CONF_TYPE && ZMOD_CONF_TYPE == LOW_POWER
#define ZMOD4410_SAMPLE_PERIOD 6
#else
#error Wrong Configuration Type
#endif

#define ZMOD4410_PID      0x2310
#define ZMOD4410_I2C_ADDR 0x32

#define ZMOD4410_H_ADDR 0x40
#define ZMOD4410_D_ADDR 0x50
#define ZMOD4410_M_ADDR 0x60
#define ZMOD4410_S_ADDR 0x68

#define ZMOD4410_PROD_DATA_LEN 7

uint8_t data_set_4410i[] = {
    0x00, 0x50,
    0x00, 0x28, 0xC3, 0xE3,
    0x00, 0x00, 0x80, 0x40
};

uint8_t data_set_4410_cont[] = {
    0xFD, 0xA8,
    0x20, 0x04, 0x20, 0x04,
    0x03,
    0x00, 0x00, 0x80, 0x08
};

uint8_t data_set_4410_lpow[] = {
    0x00, 0x50, 0xFD, 0XA8,
    0x00, 0xCD, 0x01, 0x9A, 0x03, 0x34,
    0x23, 0x03,
    0x00, 0x00, 0x0A, 0x41, 0x0A, 0x41,
    0x00, 0x41, 0x00, 0x41, 0x00, 0x49,
    0x00, 0x49, 0x00, 0x51, 0x00, 0x09,
    0x00, 0x49, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x80, 0x40
};

zmod4xxx_conf zmod_sensor_type[] = {
    [INIT] = {
        .start = 0x80,
        .h = { .addr = ZMOD4410_H_ADDR, .len = 2, .data_buf = &data_set_4410i[0]},
        .d = { .addr = ZMOD4410_D_ADDR, .len = 2, .data_buf = &data_set_4410i[2]},
        .m = { .addr = ZMOD4410_M_ADDR, .len = 2, .data_buf = &data_set_4410i[4]},
        .s = { .addr = ZMOD4410_S_ADDR, .len = 4, .data_buf = &data_set_4410i[6]},
        .r = { .addr = 0x97, .len = 4},
    },

    [CONTINUOUS] = {
        .start = 0xC0,
        .h = {.addr = ZMOD4410_H_ADDR, .len = 2, .data_buf = &data_set_4410_cont[0]},
        .d = {.addr = ZMOD4410_D_ADDR, .len = 4, .data_buf = &data_set_4410_cont[2]},
        .m = {.addr = ZMOD4410_M_ADDR, .len = 1, .data_buf = &data_set_4410_cont[6]},
        .s = {.addr = ZMOD4410_S_ADDR, .len = 4, .data_buf = &data_set_4410_cont[7]},
        .r = {.addr = 0x99, .len = 2},
        .prod_data_len = ZMOD4410_PROD_DATA_LEN,
    },
    [LOW_POWER] = {
        .start = 0x80,
        .h = {.addr = ZMOD4410_H_ADDR, .len = 4,  .data_buf = &data_set_4410_lpow[0]},
        .d = {.addr = ZMOD4410_D_ADDR, .len = 6,  .data_buf = &data_set_4410_lpow[4]},
        .m = {.addr = ZMOD4410_M_ADDR, .len = 2,  .data_buf = &data_set_4410_lpow[10]},
        .s = {.addr = ZMOD4410_S_ADDR, .len = 28, .data_buf = &data_set_4410_lpow[12]},
        .r = {.addr = 0xA9, .len = 2},
        .prod_data_len = ZMOD4410_PROD_DATA_LEN,
    }
};

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* ZMOD4XXX_CONFIG_H */