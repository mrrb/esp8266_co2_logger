/**
 * \file ccs811_defs.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-03-16
 */

#ifndef CCS811_DEFS_H
#define CCS811_DEFS_H

#include <osapi.h>
#include <c_types.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Sensor */
#define CCS811_SENSOR_ID 0x81

/* I2C address */
#define CCS811_I2C_ADDR_LOW  0x5A
#define CCS811_I2C_ADDR_HIGH 0x5B

/* I2C address */
#define CCS811_MODE_FW_BOOT  0x00
#define CCS811_MODE_FW_APP   0x01

/* Drive mode */
#define CCS811_DRIVE_MODE_IDLE 0b000            /* Mode 0 – Idle (disabled) */
#define CCS811_DRIVE_MODE_1S   0b001            /* Mode 1 – Measure every second */
#define CCS811_DRIVE_MODE_10S  0b010            /* Mode 2 – Measure every 10 seconds */
#define CCS811_DRIVE_MODE_60S  0b011            /* Mode 3 – Measure every 60 seconds */
#define CCS811_DRIVE_MODE_CON  0b100            /* Mode 4 – Constant power. Measure every 250 ms */

/* Interrupt */
#define CCS811_INT_DISABLE     0b0              /* No interrupt signal generated */
#define CCS811_INT_ENABLE      0b1              /* 'nINT' asserted when a new sample is ready */

#define CCS811_INT_THRESH_OFF  0b0              /* Normal interrupt assertion */
#define CCS811_INT_THRESH_ON   0b1              /* Only assert interrupt if the new data crosses a given threshold */

/* App & Bootloader common registers */
#define CCS811_REG_STATUS                0x00
#define CCS811_REG_HW_ID                 0x20
#define CCS811_REG_HW_VERSION            0x21
#define CCS811_REG_FW_BOOT_VERSION       0x23
#define CCS811_REG_FW_APP_VERSION        0x24
#define CCS811_REG_ERROR_ID              0xE0
#define CCS811_REG_SW_RESET              0xFF

/* App FW register map */
#define CCS811_APP_REG_STATUS            CCS811_REG_STATUS          /* Sensor status [R - 1byte] */
#define CCS811_APP_REG_MEAS_MODE         0x01                       /* Measurement mode [R/W - 1byte] */
#define CCS811_APP_REG_ALG_RESULT_DATA   0x02                       /* Algorithm result [R - max 8bytes] */
#define CCS811_APP_REG_RAW_DATA          0x03                       /* Raw ADC data [R - 2bytes] */
#define CCS811_APP_REG_ENV_DATA          0x05                       /* Temp. & hum. compensation values [W - 4bytes] */
#define CCS811_APP_REG_THRESHOLDS        0x10                       /* Interrupts hresholds [W - 4bytes] */
#define CCS811_APP_REG_BASELINE          0x11                       /* Current baseline [R/W - 2bytes] */
#define CCS811_APP_REG_HW_ID             CCS811_REG_HW_ID           /* HW ID, should be 0x81 [R - 1byte] */
#define CCS811_APP_REG_HW_VERSION        CCS811_REG_HW_VERSION      /* HW version, 0x1X [R - 1byte] */
#define CCS811_APP_REG_FW_BOOT_VERSION   CCS811_REG_FW_BOOT_VERSION /* FW bootloader version [R - 2bytes] */
#define CCS811_APP_REG_FW_APP_VERSION    CCS811_REG_FW_APP_VERSION  /* FW application version [R - 2bytes] */
#define CCS811_APP_REG_INTERNAL_STATE    0xA0                       /* Internal state [R - 1byte] */
#define CCS811_APP_REG_ERROR_ID          CCS811_REG_ERROR_ID        /* Error ID [R - 1byte] */
#define CCS811_APP_REG_SW_RESET          CCS811_REG_SW_RESET        /* Reset if the sequence 0x11 0xE5 0x72 0x8A is written [W - 4bytes] */

/* Bootloader FW register map */
#define CCS811_BOOT_REG_STATUS           CCS811_REG_STATUS          /* Status [R - 1byte] */
#define CCS811_BOOT_REG_HW_ID            CCS811_REG_HW_ID           /* HW ID, should be 0x81 [R - 1byte] */
#define CCS811_BOOT_REG_HW_VERSION       CCS811_REG_HW_VERSION      /* HW version, 0x1X [R - 1byte] */
#define CCS811_BOOT_REG_FW_BOOT_VERSION  CCS811_REG_FW_BOOT_VERSION /* FW bootloader version [R - 2bytes] */
#define CCS811_BOOT_REG_FW_APP_VERSION   CCS811_REG_FW_APP_VERSION  /* FW application version [R - 2bytes] */
#define CCS811_BOOT_REG_ERROR_ID         CCS811_REG_ERROR_ID        /* Error ID [R - 1byte] */
#define CCS811_BOOT_REG_APP_ERASE        0xF1                       /* Erase app FW if the sequence 0xE7 0xA7 0xE6 0x09 is written [W - 4bytes] */
#define CCS811_BOOT_REG_APP_DATA         0xF2                       /* New app FW code [W - 9bytes] */
#define CCS811_BOOT_REG_APP_VERIFY       0xF3                       /* Check if the given app FW is valid [-] */
#define CCS811_BOOT_REG_APP_START        0xF4                       /* Start main app FW [-] */
#define CCS811_BOOT_REG_SW_RESET         CCS811_REG_SW_RESET        /* Reset if the sequence 0x11 0xE5 0x72 0x8A is written [W - 4bytes] */

/* Reset sequence */
#define CCS811_RESET_SEQ0 0x11
#define CCS811_RESET_SEQ1 0xE5
#define CCS811_RESET_SEQ2 0x72
#define CCS811_RESET_SEQ3 0x8A

/* Status macros */
#define CCS811_STATUS_FW_MODE(status)           ((status & 0x80) >> 7)
#define CCS811_STATUS_APP_ERASE(status)         ((status & 0x40) >> 6)
#define CCS811_STATUS_APP_VERIFY(status)        ((status & 0x20) >> 5)
#define CCS811_STATUS_APP_VALID(status)         ((status & 0x10) >> 4)
#define CCS811_STATUS_DATA_READY(status)        ((status & 0x08) >> 3)
#define CCS811_STATUS_ERROR(status)             ((status & 0x01) >> 0)

/* Error id macros */
#define CCS811_ERROR_ID_WRITE_REG_INVALID(eid)  ((eid & 0x80) >> 7)
#define CCS811_ERROR_ID_READ_REG_INVALID(eid)   ((eid & 0x40) >> 6)
#define CCS811_ERROR_ID_MEASMODE_INVALID(eid)   ((eid & 0x20) >> 5)
#define CCS811_ERROR_ID_MAX_RESISTANCE(eid)     ((eid & 0x10) >> 4)
#define CCS811_ERROR_ID_HEATER_FAULT(eid)       ((eid & 0x08) >> 3)
#define CCS811_ERROR_ID_HEATER_SUPPLY(eid)      ((eid & 0x04) >> 2)

/* Measure mode macros */
#define CCS811_MEAS_MODE_DRIVE_MODE(code)       ((code & 0x70) >> 4)
#define CCS811_MEAS_MODE_INTERRUPT(code)        ((code & 0x08) >> 3)
#define CCS811_MEAS_MODE_THRESHOLD(code)        ((code & 0x04) >> 2)

/* RAW data macros */
#define CCS811_RAW_DATA_CURRENT(value)          ((value & 0xFC00) >> 10)
#define CCS811_RAW_DATA_ADC(value)              ((value & 0x03FF) >> 0)

/**
 * \brief           CCS811 results
 */
typedef enum {
    CCS811_OK,                                  /*!< Everything Ok */
    CCS811_ERR_I2C,                             /*!< I2C comm error */
    CCS811_ERR_HAL,                             /*!< Invalid HAL function pointers (read, write, delay) */
    CCS811_ERR_INVALID_SENSOR,                  /*!< Invalid sensor */
} ccs811_result_t;

/**
* \brief            Pointer to i2c read/write function
* \param[in]        addr: 7-bit I2C address of the sensor
* \param[in]        reg_addr: address of internal register to read/write
* \param[in,out]    data: pointer to the read/write data value
* \param[in]        len: number of bytes to read/write
* \return           \ref CCS811_OK on success, \ref CCS811_ERR_I2C on error
*/
typedef ccs811_result_t (*ccs811_i2c_p_t)(uint8_t addr, uint8_t reg_addr, uint8_t* data_buf, uint8_t len);

/**
 * \brief           Pointer to platform delay function
 * \param[in]       ms: delay in milliseconds
 */
typedef void (*ccs811_delay_p_t)(uint32_t ms);

/**
 * \brief           CCS811 device structure
 */
typedef struct {
    uint8_t i2c_addr;                           /*!< Sensor I2C address */
    ccs811_i2c_p_t i2c_read;                    /*!< I2C read function pointer */
    ccs811_i2c_p_t i2c_write;                   /*!< I2C write function pointer */
    ccs811_delay_p_t delay_ms;                  /*!< Delay function pointer */

    uint8_t hw_id;                              /*!< Sensor HW id */
    uint8_t hw_version;                         /*!< Sensor HW version */
    uint16_t fw_boot_version;                   /*!< Bootloader FW version */
    uint16_t fw_app_version;                    /*!< App FW version */
} ccs811_dev_t;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CCS811_DEFS_H */
