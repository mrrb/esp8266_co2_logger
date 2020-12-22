/*
 * crc8.h
 *
 *  Created on: Oct 8, 2019
 *      Author: Mikhail Kalina
 */

#ifndef APP_CRC8_CRC8_H_
#define APP_CRC8_CRC8_H_

#include <stddef.h>
#include <c_types.h>
#include <stdbool.h>


/**
 * \brief       CRC calculation for a memory area without an auxiliary table
 * \param mem   memory pointer for calculating CRC
 * \param len   number of bytes to calculate CRC
 * \param crc   pointer to the initial CRC value (before the function call)
 *              and the updated CRC value (after the function call)
 * \return      none
 */
void crc8_simple(void const *mem, size_t len, uint8_t *crc);


/**
 * \brief       CRC calculation for one byte without auxiliary table
 * \param byte  data byte for calculating CRC
 * \param crc   pointer to the initial CRC value (before the function call)
 *              and the updated CRC value (after the function call)
 * \return      none
 */
void crc8_simple_byte(uint8_t byte, uint8_t *crc);


/**
 * \brief       CRC calculation for a memory area with an auxiliary table
 * \param mem   memory pointer for calculating CRC
 * \param len   number of bytes to calculate CRC
 * \param crc   pointer to the initial CRC value (before the function call)
 *              and the updated CRC value (after the function call)
 * \return      none
 */
void crc8_fast(void const *mem, size_t len, uint8_t *crc);


/**
 * \brief       CRC calculation for one byte with auxiliary table
 * \param byte  data byte for calculating CRC
 * \param crc   pointer to the initial CRC value (before the function call)
 *              and the updated CRC value (after the function call)
 * \return      none
 */
void crc8_fast_byte(uint8_t byte, uint8_t *crc);

#endif /* APP_CRC8_CRC8_H_ */
