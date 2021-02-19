/**
 * \file extra_time.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-02-02
 */

#include <user_interface.h>
#include <c_types.h>
#include <osapi.h>

#include "extra_time.h"

void
delay_ms(uint_fast16_t ms) {
    uint_fast16_t n_iters   = (1000 * ((uint_fast32_t) ms)) / 65535;
    uint_fast16_t nxt_delay = (1000 * ((uint_fast32_t) ms)) % 65535;

    for (size_t i = 0; i < n_iters; ++i) {
        os_delay_us(65535);
        system_soft_wdt_feed();
    }

    os_delay_us(nxt_delay);
}
