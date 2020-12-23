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

/**
 * \brief           SCD30 available commands
 */
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


/**
 * \brief           Validate, using CRC8, a group of bytes
 * \param[in]       data: Received data
 * \param[in]       offset: Jump to the `n` group within the data
 * \return          CRC comparison result (1 valid, 0 not valid)
 */
uint8_t ICACHE_FLASH_ATTR check_valid_crc(uint8_t* data, uint8_t offset);

/**
 * \brief           Send a command to the SCD30 module
 * \param[in]       cmd: SCD30 command (view interface description doc)
 * \param[in]       send_argument: '1' (with argument) or '0' (without argument)
 * \param[in]       argument: If 'send_argument'=='1'. Argument to send w/ the command
 * \return          Result of the operation
 */
uint8_t scd30_write(uint16_t cmd, uint8_t send_argument, uint16_t argument);

/**
 * \brief           Read the result of a command from the SCD30 module
 * \param[in]       cmd: SCD30 command (view interface description doc)
 * \param[out]      data: Pointer to the buffer where the data is going to be stored
 * \param[in]       data_len: Number of bytes to be read
 * \return          Result of the operation
 */
uint8_t scd30_read(uint16_t cmd, uint8_t* data, size_t data_len);

/**
 * \brief           Start the continuous measurement mode
 * \param[in]       pressure: Optional ambient pressure compensation
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_start_continuous_measurement(uint16_t pressure);

/**
 * \brief           Stop the continuous measurement mode
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_stop_continuous_measurement();

/**
 * \brief           Set the measurement interval (for continuous measurement mode)
 * \param[in]       interval: Time interval. From 2 to 1800 seconds
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_set_measurement_interval(uint16_t interval);

/**
 * \brief           Get the current measurement interval (for continuous measurement mode)
 * \param[out]      interval: Time interval. From 2 to 1800 seconds
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_get_measurement_interval(uint16_t* interval);

/**
 * \brief           Check if a measurement  can  be read from the sensor’s buffer
 * \param[out]      data_ready: Check result
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_check_data_ready(uint8_t* data_ready);

/**
 * \brief           Get a measurement from the sensor
 * \param[out]      co2: Raw CO2 value. Need to be converted to float
 * \param[out]      t: Raw temperature value. Need to be converted to float
 * \param[out]      rh: Raw humidity value. Need to be converted to float
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_read_measurement(uint32_t* co2, uint32_t* t, uint32_t* rh);

/**
 * \brief           (De)activate the automatic self calibration funcionality (ASC)
 * \param[in]       status: Active (1) or deactivate (0)
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_toggle_automatic_self_calibration(uint8_t status);

/**
 * \brief           Set a compensation value for sensor drifts when a valid
 *                  reference CO2 value is available
 * \param[in]       concentration: CO2 concentration in ppm
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_set_forced_recalibration_value(uint16_t concentration);

/**
 * \brief           Get the current CO2 compensation value
 * \param[out]      concentration: CO2 concentration in ppm
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_get_forced_recalibration_value(uint16_t* concentration);

/**
 * \brief           Set the temperature compensation value
 * \param[in]       offset: Temperature offset [°C x 100], i.e. One tick corresponds to 0.01°C
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_set_temperature_offset(uint16_t offset);

/**
 * \brief           Get the current temperature compensation value
 * \param[out]      offset: Temperature offset [°C x 100], i.e. One tick corresponds to 0.01°C
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_get_temperature_offset(uint16_t* offset);

/**
 * \brief           Set the sensor altitude to compensate deviations
 *                  This value will be disregarded when an ambient pressure is given
 * \param[in]       altitude: Height over sea level in [m] above 0
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_set_altitude_compensation_value(uint16_t altitude);

/**
 * \brief           Get the current sensor altitude to compensate deviations
 * \param[out]      altitude: Height over sea level in [m] above 0
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_get_altitude_compensation_value(uint16_t* altitude);

/**
 * \brief           Get the module firmware version
 * \param[out]      version_major: Firmware version [major]
 * \param[out]      version_minor: Firmware version [minor]
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_get_firmware_version(uint8_t* version_major, uint8_t* version_minor);

/**
 * \brief           Soft reset the module
 * \return          Result of the operation
 */
uint8_t ICACHE_FLASH_ATTR scd30_soft_reset();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SCD30_H */
