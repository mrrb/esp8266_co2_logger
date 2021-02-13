/**
 * \file extra_time.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-01-31
 */

#ifndef EXTRA_TIME_H
#define EXTRA_TIME_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
    #define delay_ms(ms) do {                                 \
        for (size_t i = 0; i < ((ms * 1000) / 65535); ++i) {  \
            os_delay_us((uint16_t) 65535);                    \
        }                                                     \
        os_delay_us(ms % (uint16_t) 65535);                   \
    } while(0)
 */

/**
 * \brief           Sleep the given time. NOTE: MAX 3000ms!!
 * \param[in]       ms: Milliseconds
 */
void delay_ms(uint_fast16_t ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* EXTRA_TIME_H */
