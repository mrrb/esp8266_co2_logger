/**
 * \file ccs811.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-03-16
 */

#ifndef CCS811_H
#define CCS811_H

#include "ccs811_defs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           Reset the sensor (into the bootloader) [boot & app]
 * \param[in]       p_dev: Sensor device struct
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_reset(ccs811_dev_t* p_dev);

/**
 * \brief           Start the app FW [boot]
 * \param[in]       p_dev: Sensor device struct
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 * \note            User need to check first if the sensor is in bootloader mode
 */
ccs811_result_t ccs811_app_start(ccs811_dev_t* p_dev);

/**
 * \brief           Read and store into p_dev id and version of the sensor (HW & FW) [boot & app]
 * \param[in]       p_dev: Sensor device struct
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_sensor_info(ccs811_dev_t* p_dev);

/**
 * \brief           Get the current sensor status [boot & app]
 * \param[in]       p_dev: Sensor device struct
 * \param[out]      p_status: Sensor status
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_status(ccs811_dev_t* p_dev, uint8_t* p_status);

/**
 * \brief           Get the last error id [boot & app]
 * \param[in]       p_dev: Sensor device struct
 * \param[out]      p_error_id: Sensor error id
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_error_id(ccs811_dev_t* p_dev, uint8_t* p_error_id);

/**
 * \brief           Set the measurement mode
 * \param[in]       p_dev: Sensor device struct
 * \param[in]       drive_mode: Operation mode
 * \param[in]       interrupt: Enable/disable interrupts
 * \param[in]       threshold: Interrupts mode
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_set_meas_mode(ccs811_dev_t* p_dev, uint8_t drive_mode, uint8_t interrupt, uint8_t threshold);

/**
 * \brief           Set the measurement mode
 * \param[in]       p_dev: Sensor device struct
 * \param[out]      p_measurement_mode: Measurement mode register
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_meas_mode(ccs811_dev_t* p_dev, uint8_t* p_measurement_mode);

/**
 * \brief           Get algorithm result data [eCO2]
 * \param[in]       p_dev: Sensor device struct
 * \param[out]      co2: eCO2 value in ppm
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_co2(ccs811_dev_t* p_dev, uint16_t* co2);

/**
 * \brief           Get algorithm result data [eCO2, eTVOCs]
 * \param[in]       p_dev: Sensor device struct
 * \param[out]      co2: eCO2 value in ppm
 * \param[out]      tvoc: eTVOCs value in ppb
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_co2_tvoc(ccs811_dev_t* p_dev, uint16_t* co2, uint16_t* tvoc);

/**
 * \brief           Get algorithm result data [eCO2, eTVOCs, status]
 * \param[in]       p_dev: Sensor device struct
 * \param[out]      co2: eCO2 value in ppm
 * \param[out]      tvoc: eTVOCs value in ppb
 * \param[out]      status: Sensor status
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_co2_tvoc_status(ccs811_dev_t* p_dev, uint16_t* co2, uint16_t* tvoc, uint8_t* status);

/**
 * \brief           Get algorithm result data [eCO2, eTVOCs, status, error]
 * \param[in]       p_dev: Sensor device struct
 * \param[out]      co2: eCO2 value in ppm
 * \param[out]      tvoc: eTVOCs value in ppb
 * \param[out]      status: Sensor status
 * \param[out]      error_id: Sensor error id
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_co2_tvoc_status_eid(ccs811_dev_t* p_dev, uint16_t* co2, uint16_t* tvoc, uint8_t* status, uint8_t* error_id);

/**
 * \brief           Get algorithm result data [all]
 * \param[in]       p_dev: Sensor device struct
 * \param[out]      co2: eCO2 value in ppm
 * \param[out]      tvoc: eTVOCs value in ppb
 * \param[out]      status: Sensor status
 * \param[out]      error_id: Sensor error id
 * \param[out]      raw_data: ADC raw data
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_co2_tvoc_status_eid_raw(ccs811_dev_t* p_dev, uint16_t* co2, uint16_t* tvoc, uint8_t* status, uint8_t* error_id, uint16_t* raw_data);

/**
 * \brief           Get sensor raw data
 * \param[in]       p_dev: Sensor device struct
 * \param[out]      raw_data: ADC raw data
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_raw_data(ccs811_dev_t* p_dev, uint16_t* raw_data);

/**
 * \brief           Set environment data
 * \param[in]       p_dev: Sensor device struct
 * \param[in]       reg_humidity: Register style humidity (bits 16-10 int, bits 9-0 fraction)
 * \param[in]       reg_temperature: Register style temperature (bits 16-10 int, bits 9-0 fraction)
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_set_env_data(ccs811_dev_t* p_dev, uint16_t reg_humidity, uint16_t reg_temperature);

/**
 * \brief           Set interrupt thresholds
 * \param[in]       p_dev: Sensor device struct
 * \param[in]       low_medium: Low to medium threshold
 * \param[in]       medium_high: Medium to high threshold
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_set_thresholds(ccs811_dev_t* p_dev, uint16_t low_medium, uint16_t medium_high);

/**
 * \brief           Set the baseline for the algorithm calculations
 * \param[in]       p_dev: Sensor device struct
 * \param[in]       baseline: Algorithm baseline
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_set_baseline(ccs811_dev_t* p_dev, uint16_t baseline);

/**
 * \brief           Get the baseline for the algorithm calculations
 * \param[in]       p_dev: Sensor device struct
 * \param[out]      baseline: Algorithm baseline
 * \return          \ref CCS811_OK on success, otherwise, any item of \ref ccs811_result_t
 */
ccs811_result_t ccs811_get_baseline(ccs811_dev_t* p_dev, uint16_t* baseline);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CCS811_H */
