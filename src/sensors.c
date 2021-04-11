/**
 * \file sensors.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-02-13
 */

#include <osapi.h>

#include "sensors.h"

#include "scd30/scd30.h"

#include "zmod4xxx/zmod4xxx.h"
#include "zmod4xxx/zmod4xxx_types.h"
#include "zmod4xxx/iaq_1st_gen.h"

#include "ccs811/ccs811.h"
#include "ccs811/ccs811_defs.h"


sensor_status_t ICACHE_FLASH_ATTR
read_scd30(scd30_result_t* p_result) {
    uint32_t co2, temp, rh;
    uint8_t status;
    uint8_t rdy = 0;

    scd30_check_data_ready(&rdy);
    if (rdy) {
        status = scd30_read_measurement(&co2, &temp, &rh);
        if (status != 1) {
            return SENSOR_READ_ERROR;
        }

        p_result->rh   = rh;
        p_result->co2  = co2;
        p_result->temp = temp;

        return SENSOR_READ_VALID;
    } else {
        return SENSOR_NOT_READY; 
    }
}

sensor_status_t ICACHE_FLASH_ATTR
calc_zmod_result(zmod4xxx_dev_t* p_zmod_dev, iaq_1st_gen_handle_t* p_iaq_handle, iaq_1st_gen_results_t* p_iaq_results, uint8_t* adc_result) {
    int8_t zmod_result;

    zmod_result = calc_iaq_1st_gen(p_iaq_handle, p_zmod_dev, adc_result, p_iaq_results);
    if (zmod_result == IAQ_1ST_GEN_STABILIZATION) {
        return SENSOR_ZMOD_STABILIZATION;
    } else if (zmod_result == IAQ_1ST_GEN_OK) {
        return SENSOR_READ_VALID;
    } else {
        return SENSOR_ZMOD_ALG_ERROR;
    }

    return SENSOR_ERR;
}

sensor_status_t ICACHE_FLASH_ATTR
read_zmod(zmod4xxx_dev_t* p_zmod_dev, iaq_1st_gen_handle_t* p_iaq_handle, iaq_1st_gen_results_t* p_iaq_results, uint8_t* adc_result) {
    int8_t zmod_result;
    uint8_t status;

    uint8_t eoc_flag = 0;
    uint8_t s_step_last = 0;
    uint8_t s_step_new = 0;

    // uint8_t adc_result[32] = {0};
    memset(adc_result, 0, SENSORS_ADC_RESULT_SIZE);


    zmod_result = zmod4xxx_start_measurement(p_zmod_dev);
    if (zmod_result) {
        return SENSOR_ZMOD_START_MEASUREMENT_ERROR;
    }

    do {
        zmod_result = zmod4xxx_read_status(p_zmod_dev, &status);
        if (zmod_result) {
            return SENSOR_READ_ERROR;
        }

        s_step_new = (status & STATUS_LAST_SEQ_STEP_MASK);
        if (s_step_new != s_step_last) {
            if (s_step_new == ((p_zmod_dev->meas_conf->s.len / 2) - 1)) {
                eoc_flag = 1;
            }
        }

        s_step_last = s_step_new;
        p_zmod_dev->delay_ms(50);
    } while (!eoc_flag);

    zmod_result = zmod4xxx_read_adc_result(p_zmod_dev, adc_result);
    if (zmod_result) {
        return SENSOR_ZMOD_ADC_ERROR;
    }

    return calc_zmod_result(p_zmod_dev, p_iaq_handle, p_iaq_results, adc_result);
}

sensor_status_t ICACHE_FLASH_ATTR
read_ccs811(ccs811_dev_t* p_ccs_dev, ccs811_data_t* p_data) {
    ccs811_result_t ccs_result;

    ccs_result = ccs811_get_co2_tvoc_status_eid_raw(p_ccs_dev, &(p_data->eco2), &(p_data->tvoc), &(p_data->status), &(p_data->error_id), &(p_data->raw_data));
    if (ccs_result != CCS811_OK) {
        return SENSOR_READ_ERROR;
    }

    if (!CCS811_STATUS_DATA_READY(p_data->status)) {
        return SENSOR_NOT_READY; 
    } else {
        return SENSOR_READ_VALID;
    }

    return SENSOR_READ_ERROR;
}
