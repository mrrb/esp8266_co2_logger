/**
 * \file main.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2020-12-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "uc_init.h"
#include "f2c/f2c.h"
#include "simple_i2c.h"
#include "crc8/crc8.h"


static volatile os_timer_t timer1;
static volatile os_timer_t timer2;

void ICACHE_FLASH_ATTR
blink(void *arg) {
    GPIO2_IN ? GPIO2_L : GPIO2_H;
    // os_printf("Now I'm %d\r\n", GPIO2_IN);
}

void ICACHE_FLASH_ATTR
test_read(void *arg) {
    uint8_t op_result;
    uint8_t data[18];
    uint32_t temp_CO2, temp_T, temp_RH;
    real32_t CO2, T, RH;
    char* txt_buff = os_malloc(sizeof(char)*50);
    char* txt_f2c;

    uint8_t crc = 0xFF;

    do {
        I2C_BEGIN_WRITE(0x61, &op_result);
        if (!op_result) {
            os_printf("FW: I2C error @WRITE0\n", op_result);
            break;
        }
        I2C_WRITE_BYTE(0xD1, &op_result);
        if (!op_result) {
            os_printf("FW: I2C error @WRITE1\n", op_result);
            break;
        }
        I2C_WRITE_BYTE(0x00, &op_result);
        if (!op_result) {
            os_printf("FW: I2C error @WRITE2\n", op_result);
            break;
        }
        I2C_STOP(&op_result);
        if (!op_result) {
            os_printf("FW: I2C error @WRITE3\n", op_result);
            break;
        }
        os_delay_us(100);

        I2C_BEGIN_READ(0x61, &op_result);
        if (!op_result) {
            os_printf("FW: I2C error @READ0\n", op_result);
            break;
        }
        I2C_READ_BYTES(data, 3, &op_result);
        if (!op_result) {
            os_printf("FW: I2C error @READ1\n", op_result);
            break;
        }
    } while(0);
    I2C_STOP(&op_result);

    crc8_fast(data, 2, &crc);
    os_printf("FW: op_result=%d\n", op_result);
    os_printf("FW: %x %x (%x) [%d.%d]\n", data[0], data[1], data[2], data[0], data[1]);
    os_printf("FW: CRC8-I2C=%x CRC8-CALC=%x. Result: %s\n", data[2], crc, (data[2] == crc) ? "\u2713" : "\u2A2F");


    // I2C_BEGIN_WRITE(0x61, &op_result);
    // I2C_WRITE_BYTE(0x03, &op_result);
    // I2C_WRITE_BYTE(0x00, &op_result);
    // I2C_STOP(&op_result);
    // os_delay_us(100);

    // I2C_BEGIN_READ(0x61, &op_result);
    // I2C_READ_BYTES(data, 18, &op_result);
    // I2C_STOP(&op_result);

    // temp_CO2 = data[0] << 24 |
    //            data[1] << 16 |
    //            data[3] << 8  |
    //            data[4];

    // temp_T = data[6] << 24 |
    //          data[7] << 16 |
    //          data[9] << 8  |
    //          data[10];

    // temp_RH = data[12] << 24 |
    //           data[13] << 16 |
    //           data[15] << 8  |
    //           data[16];

    // CO2 = (real32_t)temp_CO2;
    // T = (real32_t)temp_T;
    // RH = (real32_t)temp_RH;

    // os_printf("RAW CO2: %2x %2x (%2x) %2x %2x (%2x)\n", data[0],  data[1],  data[2],  data[3],  data[4],  data[5]);
    // os_printf("RAW T:   %2x %2x (%2x) %2x %2x (%2x)\n", data[6],  data[7],  data[8],  data[9],  data[10], data[11]);
    // os_printf("RAW RH:  %2x %2x (%2x) %2x %2x (%2x)\n", data[12], data[13], data[14], data[15], data[16], data[17]);


    // // TODO - Fix this mess
    // txt_f2c = nf2c(CO2, txt_buff, 50);
    // os_printf("CO2: %s ppm\n", txt_f2c);
 
    // txt_f2c = nf2c(T, txt_buff, 50);
    // os_printf("T:   %s ºC\n", txt_f2c);

    // txt_f2c = nf2c(RH, txt_buff, 50);
    // os_printf("RH:  %s %\n", txt_f2c);

    os_free(txt_buff);
}

void ICACHE_FLASH_ATTR
user_pre_init() {}

void ICACHE_FLASH_ATTR
user_init()
{
    uc_init_uart();
    uc_init_wifi();
    uc_init_i2c();

    // espconn_gethostbyname();
    // ICACHE_RODATA_ATTR

    gpio_init();
    GPIO2_OUTPUT_SET;

    // User tasks? system_os_task(...)
    // os_timer_setfn((os_timer_t*)&timer1, (os_timer_func_t *)blink, NULL);
    // os_timer_arm((os_timer_t*)&timer1, 500, 1);

    os_timer_setfn((os_timer_t*)&timer2, (os_timer_func_t *)test_read, NULL);
    os_timer_arm((os_timer_t*)&timer2, 3000, 1);
}
