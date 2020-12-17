/**
 * \file status.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief Status codes. Header file.
 * \version 0.1
 * \date 2020-12-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef STATUS_H
#define STATUS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum status {
    STA_ERR = 0,
    STA_OK,
} status_t;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STATUS_H */
