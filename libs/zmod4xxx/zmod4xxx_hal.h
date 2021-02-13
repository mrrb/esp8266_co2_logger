/**
 * \file zmod4xxx_hal.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-01-13
 */

#ifndef ZMOD4XXX_HAL_H
#define ZMOD4XXX_HAL_H

#include <c_types.h>
#include "zmod4xxx_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

zmod4xxx_err zmod4xxx_init_hw(zmod4xxx_dev_t* dev);

int8_t zmod4xxx_i2c_read(uint8_t addr, uint8_t reg_addr, uint8_t *data_buf, uint8_t len);
int8_t zmod4xxx_i2c_write(uint8_t addr, uint8_t reg_addr, uint8_t *data_buf, uint8_t len);
void   zmod4xxx_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* ZMOD4XXX_HAL_H */