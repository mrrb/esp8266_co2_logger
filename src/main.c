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

#include <c_types.h>
#include "uc_init.h"
#include "f2c/f2c.h"
#include "scd30/scd30.h"


static volatile os_timer_t timer1;
static volatile os_timer_t timer2;

void ICACHE_FLASH_ATTR
blink(void *arg) {
    GPIO2_IN ? GPIO2_L : GPIO2_H;
    // os_printf("Now I'm %d\r\n", GPIO2_IN);
}

void ICACHE_FLASH_ATTR
test_read(void *arg) {
    uint8_t status = 0;
    uint8_t rdy = 0;
    uint8_t fw_major, fw_minor;
    uint32_t co2, t, rh;

    char* txt_buff = os_malloc(sizeof(char)*F2C_CHAR_BUFF_SIZE);
    char* txt_f2c;

    for (size_t i = 0; i < F2C_CHAR_BUFF_SIZE; ++i) {
        txt_buff[i] = 0;
    }

    status = scd30_get_firmware_version(&fw_major, &fw_minor);
    os_printf("FW:  %d.%d (%d)\n", fw_major, fw_minor, status);

    status = scd30_check_data_ready(&rdy);
    if (rdy) {
        os_printf("Measure ready.\n", rdy, status);
        status = scd30_read_measurement(&co2, &t, &rh);

        txt_f2c = f2c(*(real32_t*) &co2, txt_buff);
        os_printf("CO2: %s (%d)\n", txt_f2c, status);

        txt_f2c = f2c(*(real32_t*) &t, txt_buff);
        os_printf("T:   %s (%d)\n", txt_f2c, status);

        txt_f2c = f2c(*(real32_t*) &rh, txt_buff);
        os_printf("RH:  %s (%d)\n\n", txt_f2c, status);
    } else {
        os_printf("Measure NOT ready.\n\n", rdy, status);
    }


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
