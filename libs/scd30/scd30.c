/**
 * \file scd30.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief SCD30 module I2C communication library. Source file.
 * \version 0.1
 * \date 2020-12-21
 * 
 */

#include <stdint.h>
#include <stddef.h>
#include "scd30/scd30.h"
#include "crc8/crc8.h"
#include "simple_i2c.h"
#include "osapi.h"


uint8_t ICACHE_FLASH_ATTR
check_valid_crc(uint8_t* data, uint8_t offset) {
    uint8_t crc = 0xFF;

    crc8_fast(data + 3*offset, 2, &crc);
    return crc = data[2];
}

uint8_t ICACHE_FLASH_ATTR
scd30_write(uint16_t cmd, uint8_t send_argument, uint16_t argument) {
    uint8_t op_result;
    uint8_t n_bytes = 2;
    uint8_t crc = 0xFF;
    uint8_t buff_out[5];

    // CMD bytes
    buff_out[0] = (uint8_t) (cmd >> 8);
    buff_out[1] = (uint8_t) cmd;

    // Argument bytes
    if (send_argument) {
        buff_out[2] = (uint8_t) (argument >> 8);
        buff_out[3] = (uint8_t) argument;

        crc8_fast(buff_out+2, 2, &crc);
        buff_out[4] = crc;

        n_bytes = 5;
    }

    I2C_BEGIN_WRITE(SCD30_I2C_ADDR, &op_result);
    if (!op_result) {
        return op_result;
    }
    I2C_WRITE_BYTES(buff_out, n_bytes, &op_result);
    if (!op_result) {
        return op_result;
    }
    I2C_STOP(&op_result);

    return op_result;
}

uint8_t ICACHE_FLASH_ATTR
scd30_read(uint16_t cmd, uint8_t* data, size_t data_len) {
    uint8_t op_result;

    op_result = scd30_write(cmd, 0, 0);
    if (!op_result) {
        return op_result;
    }
    os_delay_us(3003);

    I2C_BEGIN_READ(SCD30_I2C_ADDR, &op_result);    if (!op_result) {
        return op_result;
    }
    I2C_READ_BYTES(data, data_len, &op_result);
    if (!op_result) {
        return op_result;
    }
    I2C_STOP(&op_result);

    for (size_t i = 0; i < data_len; ) {
        if (!check_valid_crc(data, i)) {
            return 0;
        }
        i = i + 3;
    }

    return op_result;
}

uint8_t ICACHE_FLASH_ATTR
scd30_start_continuous_measurement(uint16_t pressure) {
    return scd30_write(SCD30_CMD_START_CONTINUOUS_MEASUREMENT, 1, pressure);
}

uint8_t ICACHE_FLASH_ATTR
scd30_stop_continuous_measurement() {
    return scd30_write(SCD30_CMD_STOP_CONTINUOUS_MEASUREMENT, 0, 0);
}

uint8_t ICACHE_FLASH_ATTR
scd30_set_measurement_interval(uint16_t interval) {
    return scd30_write(SCD30_CMD_SET_MEASUREMENT_INTERVAL, 1, interval);
}

uint8_t ICACHE_FLASH_ATTR
scd30_get_measurement_interval(uint16_t* interval) {
    uint8_t data[3];
    uint8_t op_result;

    op_result = scd30_read(SCD30_CMD_SET_MEASUREMENT_INTERVAL, data, 3);
    if (!op_result) {
        return op_result;
    }

    *interval = data[0] << 8 |
                data[1];
    return 1;
}

uint8_t ICACHE_FLASH_ATTR
scd30_check_data_ready(uint8_t* data_ready) {
    uint8_t data[3];
    uint8_t op_result;

    op_result = scd30_read(SCD30_CMD_GET_DATA_READY_STATUS, data, 3);
    if (!op_result) {
        return op_result;
    }

    *data_ready = data[1];
    return 1;
}

uint8_t ICACHE_FLASH_ATTR
scd30_read_measurement(real32_t* co2, real32_t* t, real32_t* rh) {
    uint8_t data[18];
    uint8_t op_result;
    uint32_t temp_co2, temp_t, temp_rh;

    op_result = scd30_read(SCD30_CMD_READ_MEASUREMENT, data, 18);
    if (!op_result) {
        return op_result;
    }

    temp_co2 = data[0] << 24 |
               data[1] << 16 |
               data[3] << 8  |
               data[4];

    temp_t = data[6] << 24 |
             data[7] << 16 |
             data[9] << 8  |
             data[10];

    temp_rh = data[12] << 24 |
              data[13] << 16 |
              data[15] << 8  |
              data[16];

    *co2 = (real32_t)temp_co2;
    *t   = (real32_t)temp_t;
    *rh  = (real32_t)temp_rh;

    return 1;
}


uint8_t ICACHE_FLASH_ATTR
scd30_toggle_automatic_self_calibration(uint8_t status) {
    return scd30_write(SCD30_CMD_TOGGLE_REFERENCE_CALCULATION, 1, status);
}

uint8_t ICACHE_FLASH_ATTR
scd30_set_forced_recalibration_value(uint16_t concentration) {
    return scd30_write(SCD30_CMD_SET_FRC_EXTERNAL_REFERENCE, 1, concentration);
}

uint8_t ICACHE_FLASH_ATTR
scd30_get_forced_recalibration_value(uint16_t* concentration) {
    uint8_t data[3];
    uint8_t op_result;

    op_result = scd30_read(SCD30_CMD_SET_FRC_EXTERNAL_REFERENCE, data, 3);
    if (!op_result) {
        return op_result;
    }

    *concentration = data[0] << 8 |
                     data[1];
    return 1;
}

uint8_t ICACHE_FLASH_ATTR
scd30_set_temperature_offset(uint16_t temperature) {
    return scd30_write(SCD30_CMD_ALTITUDE_COMPENSATION, 1, temperature);
}

uint8_t ICACHE_FLASH_ATTR
scd30_get_temperature_offset(uint16_t* temperature) {
    uint8_t data[3];
    uint8_t op_result;

    op_result = scd30_read(SCD30_CMD_ALTITUDE_COMPENSATION, data, 3);
    if (!op_result) {
        return op_result;
    }

    *temperature = data[0] << 8 |
                   data[1];
    return 1;
}

uint8_t ICACHE_FLASH_ATTR
scd30_set_altitude_compensation_value(uint16_t altitude) {
    return scd30_write(SCD30_CMD_ALTITUDE_COMPENSATION, 1, altitude);
}

uint8_t ICACHE_FLASH_ATTR
scd30_get_altitude_compensation_value(uint16_t* altitude) {
    uint8_t data[3];
    uint8_t op_result;

    op_result = scd30_read(SCD30_CMD_ALTITUDE_COMPENSATION, data, 3);
    if (!op_result) {
        return op_result;
    }

    *altitude = data[0] << 8 |
                data[1];
    return 1;
}

uint8_t ICACHE_FLASH_ATTR
scd30_get_firmware_version(uint16_t* version) {
    uint8_t data[3];
    uint8_t op_result;

    op_result = scd30_read(SCD30_CMD_FIRMWARE_VERSION, data, 3);
    if (!op_result) {
        return op_result;
    }

    *version = data[0] << 8 |
               data[1];
    return 1;
}

uint8_t ICACHE_FLASH_ATTR
scd30_soft_reset(uint16_t* version) {
    return scd30_write(SCD30_CMD_SOFT_RESET, 0, 0);
}
