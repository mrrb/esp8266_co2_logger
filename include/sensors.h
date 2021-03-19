/**
 * \file sensors.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-02-13
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <c_types.h>

#include "zmod4xxx/zmod4xxx_types.h"
#include "zmod4xxx/iaq_2nd_gen.h"

#include "ccs811/ccs811_defs.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    uint32_t temp;
    uint32_t co2;
    uint32_t rh;
} scd30_result_t;

typedef struct {
    uint16_t eco2;
    uint16_t tvoc;
    uint8_t  status;
    uint8_t  error_id;
    uint16_t raw_data;
} ccs811_data_t;

typedef enum sensor_status {
    SENSOR_OK,
    SENSOR_READ_VALID,
    SENSOR_READ_ERROR,
    SENSOR_NOT_READY,
    SENSOR_ZMOD_ADC_ERROR,
    SENSOR_ZMOD_ALG_ERROR,
    SENSOR_ZMOD_STABILIZATION,
    SENSOR_ZMOD_START_MEASUREMENT_ERROR,
} sensor_status_t;

sensor_status_t read_scd30(scd30_result_t* p_result);
sensor_status_t read_zmod(zmod4xxx_dev_t* p_zmod_dev, iaq_2nd_gen_handle_t* p_iaq_handle, iaq_2nd_gen_results_t* p_iaq_results);
sensor_status_t read_ccs811(ccs811_dev_t* p_ccs_dev, ccs811_data_t* p_result);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SENSORS_H */

