/**
 * \file ccs811_hal.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-03-17
 */

#include <osapi.h>

#include "ccs811.h"
#include "ccs811_defs.h"

#include "simple_i2c.h"
#include "extra_time.h"
#include "fast_gpio.h"


/**
 * \brief           I2C read
 * \param[in]       addr: 7-bit I2C slave address of the sensor
 * \param[in]       reg_addr: address of internal register to read
 * \param[out]      data_buf: pointer to the read data value
 * \param[in]       len: number of bytes to read
 * \return          error code
 */
ccs811_result_t ICACHE_FLASH_ATTR
ccs811_i2c_read(uint8_t addr, uint8_t reg_addr, uint8_t* data_buf, uint8_t len) {
    uint8_t status;
    uint8_t rd_bytes = 0;
    uint8_t* p_data;

    GPIO13_L;
    os_delay_us(50);

    I2C_START_WRITE(addr, &status);
    if (!status) {
        I2C_STOP(&status);
        GPIO13_H;
        return CCS811_ERR_I2C;
    }
    I2C_WRITE_BYTE(reg_addr, &status);
    if (!status) {
        I2C_STOP(&status);
        GPIO13_H;
        return CCS811_ERR_I2C;
    }
    I2C_STOP(&status);


    while (rd_bytes < len) {
        I2C_START_READ(addr, &status);
        if (!status) {
            I2C_STOP(&status);
            GPIO13_H;
            return CCS811_ERR_I2C;
        }

        p_data = &data_buf[rd_bytes];
        I2C_READ_BYTES(p_data, 2, &status);
        if (!status) {
            I2C_STOP(&status);
            GPIO13_H;
            return CCS811_ERR_I2C;
        }

        rd_bytes += 2;

        I2C_STOP(&status);
    }

    if ((len - rd_bytes) == 1) {
        I2C_START_READ(addr, &status);
        if (!status) {
            I2C_STOP(&status);
            GPIO13_H;
            return CCS811_ERR_I2C;
        }

        p_data = &data_buf[rd_bytes];
        I2C_READ_BYTE(p_data, &status);
        if (!status) {
            I2C_STOP(&status);
            GPIO13_H;
            return CCS811_ERR_I2C;
        }

        I2C_STOP(&status);
    }


    GPIO13_H;
    os_delay_us(125);

    return CCS811_OK;
}

/**
 * \brief           I2C write
 * \param[in]       addr: 7-bit I2C slave address of the sensor
 * \param[in]       reg_addr: address of internal register to write
 * \param[in]       data_buf: pointer to the write data value
 * \param[in]       len: number of bytes to write
 * \return          error code
 */
ccs811_result_t ICACHE_FLASH_ATTR
ccs811_i2c_write(uint8_t addr, uint8_t reg_addr, uint8_t *data_buf, uint8_t len) {
    uint8_t status;

    GPIO13_L;

    I2C_START_WRITE(addr, &status);
    if (!status) {
        I2C_STOP(&status);
        GPIO13_H;
        return CCS811_ERR_I2C;
    }

    I2C_WRITE_BYTE(reg_addr, &status);
    if (!status) {
        I2C_STOP(&status);
        GPIO13_H;
        return CCS811_ERR_I2C;
    }

    if (len > 0) {
        I2C_WRITE_BYTES(data_buf, len, &status);
        if (!status) {
            I2C_STOP(&status);
            GPIO13_H;
            return CCS811_ERR_I2C;
        }
    }

    I2C_STOP(&status);

    GPIO13_H;
    os_delay_us(125);

    return CCS811_OK;
}

/**
 * \brief           Wrapper for the delay_ms function
 * \param[in]       ms: delay in milliseconds
 */
void ICACHE_FLASH_ATTR
ccs811_delay_ms(uint32_t ms) {
    delay_ms(ms);
}

/**
 * \brief           Fill the dev struct with the HW related stuff
 * \param[out]      dev: ccs811 device struct 
 * \return          error code 
 */
ccs811_result_t ICACHE_FLASH_ATTR
ccs811_init_hw(ccs811_dev_t* dev) {
    GPIO13_OUTPUT_SET;
    GPIO13_H;

    dev->delay_ms  = ccs811_delay_ms;
    dev->i2c_write = ccs811_i2c_write;
    dev->i2c_read  = ccs811_i2c_read;

    ccs811_delay_ms(20);

    return CCS811_OK;
}
