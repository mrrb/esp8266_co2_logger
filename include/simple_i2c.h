/**
 * \file simple_i2c.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief Functions to quickly use the SW_I2C communication. Header file.
 * \version 0.1
 * \date 2020-12-15
 * 
 */

#ifndef SIMPLE_I2C_H
#define SIMPLE_I2C_H

#include <stdint.h>
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum i2c_start_op {
    I2C_OP_WRITE = 0,
    I2C_OP_READ  = 1,
} i2c_start_op_t;

/**
 * \brief           Init the software I2C bus
 * \param[out]      p_result: Result of the operation
 * \hideinitializer
 */
#define I2C_INIT(p_result) do { \
    i2c_master_gpio_init();     \
    i2c_master_init();          \
    *p_result = 1;              \
} while(0)

/**
 * \brief           Begin a I2C transaction
 * \param[in]       addr: I2C slave address
 * \param[in]       op: i2c_start_op_t. I2C operation read/write
 * \param[out]      p_result: Result of the operation
 * \hideinitializer
 */
#define I2C_BEGIN(addr, op, p_result) do { \
    i2c_master_start();                    \
    i2c_master_writeByte((addr<<1) + op);  \
    *p_result = i2c_master_checkAck();     \
} while(0)

/**
 * \brief           Begin a READ I2C transaction
 * \param[in]       addr: I2C slave address
 * \param[out]      p_result: Result of the operation
 * \hideinitializer
 */
#define I2C_BEGIN_READ(addr, p_result) do { \
    I2C_BEGIN(addr, I2C_OP_READ, p_result); \
} while(0)

/**
 * \brief           Begin a WRITE I2C transaction
 * \param[in]       addr: I2C slave address
 * \param[out]      p_result: Result of the operation
 * \hideinitializer
 */
#define I2C_BEGIN_WRITE(addr, p_result) do { \
    I2C_BEGIN(addr, I2C_OP_WRITE, p_result); \
} while(0)

/**
 * \brief           Stop the I2C transaction
 * \param[out]      p_result: Result of the operation
 * \hideinitializer
 */
#define I2C_STOP(p_result) do { \
    i2c_master_stop();          \
    *p_result = 1;              \
} while(0)

/**
 * \brief           Send a byte to the I2C slave
 * \param[in]       data: Byte to send
 * \param[out]      p_result: Result of the operation
 * \hideinitializer
 */
#define I2C_WRITE_BYTE(data, p_result) do { \
    i2c_master_writeByte(data);             \
    *p_result = i2c_master_checkAck();      \
} while(0)

/**
 * \brief           Send `data_len` byte(s) to the I2C slave
 * \param[in]       p_data: Byte(s) to send
 * \param[in]       data_len: Number of bytes
 * \param[out]      p_result: Result of the operation
 * \hideinitializer
 */
#define I2C_WRITE_BYTES(p_data, data_len, p_result) do { \
    if (data == NULL || data_len < 1) {                  \
        *p_result = 0;                                   \
    } else {                                             \
        for (size_t i = 0; i < data_len; ++i) {          \
            *p_result = I2C_WRITE_BYTE(*(data++));       \
            if (!(*p_result)) {                          \
                break;                                   \
            }                                            \
        }                                                \
    }                                                    \
} while(0)

/**
 * \brief           Read a byte from the I2C slave
 * \param[in]       p_data: Readed byte
 * \param[out]      p_result: Result of the operation
 * \hideinitializer
 */
#define I2C_READ_BYTE(p_data, p_result) do { \
    if (data == NULL) {                      \
        *p_result = 0;                       \
    } else {                                 \
        *p_data = i2c_master_readByte();     \
        i2c_master_send_nack();              \
        *p_result = 1;                       \
    }                                        \
} while(0)

/**
 * \brief           Read `data_len` byte(s) from the I2C slave
 * \param[in]       p_data: Readed bytes(s)
 * \param[in]       data_len: Number of bytes
 * \param[out]      p_result: Result of the operation
 * \hideinitializer
 */
#define I2C_READ_BYTES(p_data, data_len, p_result) do { \
    if (data == NULL || data_len < 1) {                 \
        *p_result = 0;                                  \
    } else {                                            \
        for (size_t i = 0; i < data_len - 1; ++i) {     \
            p_data[i] = i2c_master_readByte();          \
            i2c_master_send_ack();                      \
        }                                               \
        p_data[data_len-1] = i2c_master_readByte();     \
        i2c_master_send_nack();                         \
        *p_result = 1;                                  \
   }                                                    \
} while(0)

#define I2C_DELAY i2c_master_wait

// inline uint8_t
// I2C_INIT() {
//     i2c_master_gpio_init();
//     i2c_master_init();

//     return 1;
// }

// inline uint8_t
// I2C_BEGIN(uint8_t addr, i2c_start_op_t op) {
//     i2c_master_start();
//     i2c_master_writeByte((addr<<1) + op);
//     return i2c_master_checkAck();
// }

// inline uint8_t
// I2C_BEGIN_READ(uint8_t addr) {
//     return I2C_BEGIN(addr, I2C_OP_READ);
// }

// inline uint8_t
// I2C_BEGIN_WRITE(uint8_t addr) {
//     return I2C_BEGIN(addr, I2C_OP_WRITE);
// }

// inline uint8_t
// I2C_STOP() {
//     i2c_master_stop();
//     return 1;
// }

// inline uint8_t
// I2C_WRITE_BYTE(uint8_t data) {
//     i2c_master_writeByte(data);
//     return i2c_master_checkAck();
// }

// inline uint8_t
// I2C_WRITE_BYTES(uint8_t* data, size_t data_len) {
//     if (data == NULL || data_len < 1) {
//         return 0;
//     }

//     uint8_t result;

//     for (size_t i = 0; i < data_len; ++i) {
//         result = I2C_WRITE_BYTE(*(data++));
//         if (!result) {
//             return result;
//         }
//     }

//     return result;
// }

// inline uint8_t
// I2C_READ_BYTE(uint8_t* data) {
//     if (data == NULL) {
//         return 0;
//     }

//     *data = i2c_master_readByte();
//     i2c_master_send_nack();

//     return 1;
// }

// inline uint8_t
// I2C_READ_BYTES(uint8_t* data, size_t data_len) {
//     if (data == NULL || data_len < 1) {
//         return 0;
//     }

//     for (size_t i = 0; i < data_len - 1; ++i) {
//         data[i] = i2c_master_readByte();
//         i2c_master_send_ack();
//     }
//     data[data_len-1] = i2c_master_readByte();
//     i2c_master_send_nack();

//     return 1;
// }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SIMPLE_I2C_H */
