/**
 * \file ccs811_hal.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-03-17
 */

#ifndef CCS811_HAL_H
#define CCS811_HAL_H

#include <c_types.h>
#include "ccs811_defs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

ccs811_result_t ccs811_init_hw(ccs811_dev_t* dev);

ccs811_result_t ccs811_i2c_read(uint8_t addr, uint8_t reg_addr, uint8_t *data_buf, uint8_t len);
ccs811_result_t ccs811_i2c_write(uint8_t addr, uint8_t reg_addr, uint8_t *data_buf, uint8_t len);
void            ccs811_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CCS811_HAL_H */