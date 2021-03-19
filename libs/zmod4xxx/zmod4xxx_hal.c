/**
 * \file zmod4xxx_hal.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-01-13
 */

#include <osapi.h>

#include "zmod4xxx_hal.h"
#include "zmod4xxx_types.h"
#include "simple_i2c.h"
#include "extra_time.h"


/**
 * \brief           I2C read
 * \param[in]       addr: 7-bit I2C slave address of the ZMOD4xxx
 * \param[in]       reg_addr: address of internal register to read
 * \param[out]      data_buf: pointer to the read data value
 * \param[in]       len: number of bytes to read
 * \return          error code
 */
int8_t ICACHE_FLASH_ATTR
zmod4xxx_i2c_read(uint8_t addr, uint8_t reg_addr, uint8_t *data_buf, uint8_t len) {
    uint8_t status;

    I2C_START_WRITE(addr, &status);
    if (!status) {
        I2C_STOP(&status);
        return ERROR_I2C;
    }
    I2C_WRITE_BYTE(reg_addr, &status);
    if (!status) {
        I2C_STOP(&status);
        return ERROR_I2C;
    }
    I2C_STOP(&status);

    I2C_START_READ(addr, &status);
    if (!status) {
        I2C_STOP(&status);
        return ERROR_I2C;
    }
    I2C_READ_BYTES(data_buf, len, &status);
    if (!status) {
        I2C_STOP(&status);
        return ERROR_I2C;
    }
    I2C_STOP(&status);

    return ZMOD4XXX_OK;
}

/**
 * \brief           I2C write
 * \param[in]       addr: 7-bit I2C slave address of the ZMOD4xxx
 * \param[in]       reg_addr: address of internal register to write
 * \param[in]       data_buf: pointer to the write data value
 * \param[in]       len: number of bytes to write
 * \return          error code
 */
int8_t ICACHE_FLASH_ATTR
zmod4xxx_i2c_write(uint8_t addr, uint8_t reg_addr, uint8_t *data_buf, uint8_t len) {
    uint8_t status;

    I2C_START_WRITE(addr, &status);
    if (!status) {
        I2C_STOP(&status);
        return ERROR_I2C;
    }
    I2C_WRITE_BYTE(reg_addr, &status);
    if (!status) {
        I2C_STOP(&status);
        return ERROR_I2C;
    }
    I2C_WRITE_BYTES(data_buf, len, &status);
    if (!status) {
        I2C_STOP(&status);
        return ERROR_I2C;
    }
    I2C_STOP(&status);

    return ZMOD4XXX_OK;
}

/**
 * \brief           ZMOD wrapper for the delay_ms function
 * \param[in]       ms: delay in milliseconds
 */
void ICACHE_FLASH_ATTR
zmod4xxx_delay_ms(uint32_t ms) {
    delay_ms(ms);
}

/**
 * \brief           Fill the dev struct with the HW related stuff
 * \param[out]      dev: zmod4xxx device struct 
 * \return          error code 
 */
zmod4xxx_err ICACHE_FLASH_ATTR
zmod4xxx_init_hw(zmod4xxx_dev_t* dev) {
    dev->delay_ms = zmod4xxx_delay_ms;
    dev->write    = zmod4xxx_i2c_write;
    dev->read     = zmod4xxx_i2c_read;

    return ZMOD4XXX_OK;
}
