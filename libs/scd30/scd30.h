/**
 * \file scd30.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief SCD30 module I2C communication library. Header file.
 * \version 0.1
 * \date 2020-12-21
 * 
 */

#ifndef SCD30_H
#define SCD30_H

#include <c_types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SCD30_I2C_ADDR 0x61

typedef enum scd30_cmd {
    SCD30_CMD_START_CONTINUOUS_MEASUREMENT = 0x0010,  /*!< Starts continuous measurement of CO2
                                                           concentration, humidity and temperature
                                                           [1.4.1] */
    SCD30_CMD_STOP_CONTINUOUS_MEASUREMENT  = 0x0104,  /*!< Stops continuous measurement [1.4.2] */
    SCD30_CMD_SET_MEASUREMENT_INTERVAL     = 0x4600,  /*!< Set the continuous measurement interval
                                                           [1.4.3] */
    SCD30_CMD_GET_DATA_READY_STATUS        = 0x0202,  /*!< Determine if a measurement can be read
                                                           from the sensor’s buffer [1.4.4] */
    SCD30_CMD_READ_MEASUREMENT             = 0x0300,  /*!< Read the last measurement [1.4.5] */
    SCD30_CMD_TOGGLE_REFERENCE_CALCULATION = 0x5306,  /*!< Toggle automatic Self-Calibration (ASC)
                                                           [1.4.6] */
    SCD30_CMD_SET_FRC_EXTERNAL_REFERENCE   = 0x5204,  /*!< Set Forced recalibration (FRC)
                                                           [1.4.6] */
    SCD30_CMD_SET_TEMPERATURE_OFFSET       = 0x5403,  /*!< Set the temperature offset of
                                                           the integrated sensor [1.4.7] */
    SCD30_CMD_ALTITUDE_COMPENSATION        = 0x5102,  /*!< Compensate deviations due to altitude
                                                           [1.4.8] */
    SCD30_CMD_FIRMWARE_VERSION             = 0xD100,  /*!< Read the firmware version of the module
                                                           [1.4.9] */
    SCD30_CMD_SOFT_RESET                   = 0xD304,  /*!< Forces the sensor into the same
                                                           state as after powering up [1.4.10] */
} scd30_cmd_t;


uint8_t scd30_write(uint16_t cmd, uint8_t send_argument, uint16_t argument);
uint8_t scd30_read(uint16_t cmd, uint8_t* data, size_t data_len);

uint8_t ICACHE_FLASH_ATTR scd30_start_continuous_measurement(uint16_t pressure);
uint8_t ICACHE_FLASH_ATTR scd30_stop_continuous_measurement();
uint8_t ICACHE_FLASH_ATTR scd30_set_measurement_interval(uint16_t interval);
uint8_t ICACHE_FLASH_ATTR scd30_get_measurement_interval(uint16_t* interval);
uint8_t ICACHE_FLASH_ATTR scd30_check_data_ready(uint8_t* data_ready);
uint8_t ICACHE_FLASH_ATTR scd30_read_measurement(uint32_t* co2, uint32_t* t, uint32_t* rh);
uint8_t ICACHE_FLASH_ATTR scd30_toggle_automatic_self_calibration(uint8_t status);
uint8_t ICACHE_FLASH_ATTR scd30_set_forced_recalibration_value(uint16_t concentration);
uint8_t ICACHE_FLASH_ATTR scd30_get_forced_recalibration_value(uint16_t* concentration);
uint8_t ICACHE_FLASH_ATTR scd30_set_temperature_offset(uint16_t offset);
uint8_t ICACHE_FLASH_ATTR scd30_get_temperature_offset(uint16_t* offset);
uint8_t ICACHE_FLASH_ATTR scd30_set_altitude_compensation_value(uint16_t altitude);
uint8_t ICACHE_FLASH_ATTR scd30_get_altitude_compensation_value(uint16_t* altitude);
uint8_t ICACHE_FLASH_ATTR scd30_get_firmware_version(uint8_t* version_major, uint8_t* version_minor);
uint8_t ICACHE_FLASH_ATTR scd30_soft_reset();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SCD30_H */
