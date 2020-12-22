/**
 * \file scd30.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief Get FW version and measurement from the SCD30 module.
 * \version 0.1
 * \date 2020-12-22
 */

#include <c_types.h>
#include "uc_init.h"
#include "f2c/f2c.h"
#include "scd30/scd30.h"


static volatile os_timer_t some_timer;

void ICACHE_FLASH_ATTR
test_read(void *arg) {
    uint8_t status, rdy;
    uint8_t fw_major, fw_minor;
    uint32_t co2, t, rh;

    char* txt_buff = os_malloc(sizeof(char)*F2C_CHAR_BUFF_SIZE);
    char* txt_f2c;

    status = scd30_get_firmware_version(&fw_major, &fw_minor);
    os_printf("FW:  %d.%d (%d)\n\n", fw_major, fw_minor, status);

    status = scd30_check_data_ready(&rdy);
    os_printf("RDY: %x (%d)\n\n", rdy, status);

    if (rdy) {
        status = scd30_read_measurement(&co2, &t, &rh);
        txt_f2c = f2c(*(real32_t*) &co2, txt_buff);
        os_printf("CO2: %s (%d)\n", txt_f2c, status);
        txt_f2c = f2c(*(real32_t*) &t, txt_buff);
        os_printf("T:   %s (%d)\n", txt_f2c, status);
        txt_f2c = f2c(*(real32_t*) &rh, txt_buff);
        os_printf("RH:  %s (%d)\n\n", txt_f2c, status);
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

    gpio_init();
    GPIO2_OUTPUT_SET;

    os_timer_setfn((os_timer_t*)&some_timer, (os_timer_func_t *)test_read, NULL);
    os_timer_arm((os_timer_t*)&some_timer, 4000, 1);
}
