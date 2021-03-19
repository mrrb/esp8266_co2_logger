/**
 * \file ccs811.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-03-16
 */

#include <osapi.h>
#include <c_types.h>

#include "ccs811.h"
#include "ccs811_defs.h"


static ccs811_result_t ICACHE_FLASH_ATTR
chk_hal_functions(ccs811_dev_t* p_dev) {
    if ((p_dev->i2c_read == NULL) || (p_dev->i2c_write == NULL) || (p_dev->delay_ms == NULL)) {
        return CCS811_ERR_HAL;
    }

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_reset(ccs811_dev_t* p_dev) {
    uint8_t reg_data[4];
    ccs811_result_t result;

    reg_data[0] = CCS811_RESET_SEQ0;
    reg_data[1] = CCS811_RESET_SEQ1;
    reg_data[2] = CCS811_RESET_SEQ2;
    reg_data[3] = CCS811_RESET_SEQ3;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Send reset cmd
    result = p_dev->i2c_write(p_dev->i2c_addr, CCS811_REG_SW_RESET, reg_data, 4);
    if (result != CCS811_OK) {
        return result;
    }

    p_dev->delay_ms(2);

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_status(ccs811_dev_t* p_dev, uint8_t* p_status) {
    uint8_t reg_data[1];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Get status register
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_REG_STATUS, reg_data, 1);
    if (result != CCS811_OK) {
        return result;
    }
    *p_status = reg_data[0];

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_error_id(ccs811_dev_t* p_dev, uint8_t* p_error_id) {
    uint8_t reg_data[1];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Get error_id register
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_REG_ERROR_ID, reg_data, 1);
    if (result != CCS811_OK) {
        return result;
    }
    *p_error_id = reg_data[0];

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_app_start(ccs811_dev_t* p_dev) {
    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Start app
    result = p_dev->i2c_write(p_dev->i2c_addr, CCS811_BOOT_REG_APP_START, NULL, 0);

    p_dev->delay_ms(2);

    return result;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_sensor_info(ccs811_dev_t* p_dev) {
    uint8_t reg_data[2];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Read and check hardware id
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_REG_HW_ID, reg_data, 1);
    if (result != CCS811_OK) {
        return result;
    }
    p_dev->hw_id = reg_data[0];

    if (reg_data[0] != CCS811_SENSOR_ID) {
        return CCS811_ERR_INVALID_SENSOR;
    }

    // Read hardware version
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_REG_HW_VERSION, reg_data, 1);
    if (result != CCS811_OK) {
        return result;
    }
    p_dev->hw_version = reg_data[0];

    // Read FW boot version
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_REG_FW_BOOT_VERSION, reg_data, 2);
    if (result != CCS811_OK) {
        return result;
    }
    p_dev->fw_boot_version = (reg_data[0] << 8) | reg_data[1];

    // Read FW app version
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_REG_FW_APP_VERSION, reg_data, 2);
    if (result != CCS811_OK) {
        return result;
    }
    p_dev->fw_app_version = (reg_data[0] << 8) | reg_data[1];

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_set_meas_mode(ccs811_dev_t* p_dev, uint8_t drive_mode, uint8_t interrupt, uint8_t threshold) {
    uint8_t reg_data[1];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Set register value
    reg_data[0] = (drive_mode << 4) | (interrupt << 3) | (threshold << 2);
    result = p_dev->i2c_write(p_dev->i2c_addr, CCS811_APP_REG_MEAS_MODE, reg_data, 1);
    if (result != CCS811_OK) {
        return result;
    }

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_meas_mode(ccs811_dev_t* p_dev, uint8_t* p_measurement_mode) {
    uint8_t reg_data[1];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Get register value
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_APP_REG_MEAS_MODE, reg_data, 1);
    if (result != CCS811_OK) {
        return result;
    }
    *p_measurement_mode = reg_data[0];  

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_co2(ccs811_dev_t* p_dev, uint16_t* co2) {
    uint8_t reg_data[4];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Get register value
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_APP_REG_ALG_RESULT_DATA, reg_data, 4);
    if (result != CCS811_OK) {
        return result;
    }
    *co2 = (reg_data[0] << 8) | reg_data[1];
    
    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_co2_tvoc(ccs811_dev_t* p_dev, uint16_t* co2, uint16_t* tvoc) {
    uint8_t reg_data[4];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Get register value
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_APP_REG_ALG_RESULT_DATA, reg_data, 4);
    if (result != CCS811_OK) {
        return result;
    }
    *co2  = (reg_data[0] << 8) | reg_data[1];
    *tvoc = (reg_data[2] << 8) | reg_data[3];
    
    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_co2_tvoc_status(ccs811_dev_t* p_dev, uint16_t* co2, uint16_t* tvoc, uint8_t* status) {
    uint8_t reg_data[5];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Get register value
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_APP_REG_ALG_RESULT_DATA, reg_data, 5);
    if (result != CCS811_OK) {
        return result;
    }
    *co2    = (reg_data[0] << 8) | reg_data[1];
    *tvoc   = (reg_data[2] << 8) | reg_data[3];
    *status =  reg_data[4];
    
    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_co2_tvoc_status_eid(ccs811_dev_t* p_dev, uint16_t* co2, uint16_t* tvoc, uint8_t* status, uint8_t* error_id) {
    uint8_t reg_data[6];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Get register value
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_APP_REG_ALG_RESULT_DATA, reg_data, 6);
    if (result != CCS811_OK) {
        return result;
    }
    *co2      = (reg_data[0] << 8) | reg_data[1];
    *tvoc     = (reg_data[2] << 8) | reg_data[3];
    *status   =  reg_data[4];
    *error_id =  reg_data[5];
    
    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_co2_tvoc_status_eid_raw(ccs811_dev_t* p_dev, uint16_t* co2, uint16_t* tvoc, uint8_t* status, uint8_t* error_id, uint16_t* raw_data) {
    uint8_t reg_data[8];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Get register value
    os_printf("Reading CCS811 data...  ");
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_APP_REG_ALG_RESULT_DATA, reg_data, 8);
    if (result != CCS811_OK) {
        os_printf("Error :(\n");
        return result;
    }
    os_printf("0->0x%02x, 0->0x%02x, 0->0x%02x, 0->0x%02x, 0->0x%02x, 0->0x%02x, 0->0x%02x, 0->0x%02x\n",
        reg_data[0], reg_data[1], reg_data[2], reg_data[3], reg_data[4], reg_data[5], reg_data[6], reg_data[7]);
    *co2      = (reg_data[0] << 8) | reg_data[1];
    *tvoc     = (reg_data[2] << 8) | reg_data[3];
    *status   =  reg_data[4];
    *error_id =  reg_data[5];
    *raw_data = (reg_data[6] << 8) | reg_data[7];
    
    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_raw_data(ccs811_dev_t* p_dev, uint16_t* raw_data) {
    uint8_t reg_data[2];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Get register value
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_APP_REG_RAW_DATA, reg_data, 2);
    if (result != CCS811_OK) {
        return result;
    }
    *raw_data = (reg_data[0] << 8) | reg_data[1];

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_set_env_data(ccs811_dev_t* p_dev, uint16_t reg_humidity, uint16_t reg_temperature) {
    uint8_t reg_data[4];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Set register values
    reg_data[0] = (reg_humidity >> 8);
    reg_data[1] = (uint8_t)reg_humidity;
    reg_data[2] = (reg_temperature >> 8);
    reg_data[3] = (uint8_t)reg_temperature;
    result = p_dev->i2c_write(p_dev->i2c_addr, CCS811_APP_REG_ENV_DATA, reg_data, 4);
    if (result != CCS811_OK) {
        return result;
    }

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_set_thresholds(ccs811_dev_t* p_dev, uint16_t low_medium, uint16_t medium_high) {
    uint8_t reg_data[4];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Set register values
    reg_data[0] = (low_medium >> 8);
    reg_data[1] = (uint8_t)low_medium;
    reg_data[2] = (medium_high >> 8);
    reg_data[3] = (uint8_t)medium_high;
    result = p_dev->i2c_write(p_dev->i2c_addr, CCS811_APP_REG_THRESHOLDS, reg_data, 4);
    if (result != CCS811_OK) {
        return result;
    }

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_set_baseline(ccs811_dev_t* p_dev, uint16_t baseline) {
    uint8_t reg_data[2];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Set register values
    reg_data[0] = (baseline >> 8);
    reg_data[1] = (uint8_t)baseline;
    result = p_dev->i2c_write(p_dev->i2c_addr, CCS811_APP_REG_BASELINE, reg_data, 2);
    if (result != CCS811_OK) {
        return result;
    }

    return CCS811_OK;
}

ccs811_result_t ICACHE_FLASH_ATTR
ccs811_get_baseline(ccs811_dev_t* p_dev, uint16_t* baseline) {
    uint8_t reg_data[2];

    ccs811_result_t result;

    // Check valid struct
    result = chk_hal_functions(p_dev);
    if (result != CCS811_OK) {
        return result;
    }

    // Set register values
    result = p_dev->i2c_read(p_dev->i2c_addr, CCS811_APP_REG_BASELINE, reg_data, 2);
    if (result != CCS811_OK) {
        return result;
    }
    *baseline = (reg_data[0] << 8) | reg_data[0];

    return CCS811_OK;
}
