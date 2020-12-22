/**
 * \file espmissing.h
 * \brief Missing function prototypes in include folders. Based on:
 *        https://github.com/chmorgan/libesphttpd/blob/master/include/libesphttpd/espmissingincludes.h
 */

#ifndef ESPMISSING_H
#define ESPMISSING_H

#include <c_types.h>

#include <eagle_soc.h>
#include <ets_sys.h>

#include "user_interface.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ets_isr_mask(unsigned intr);
void ets_isr_unmask(unsigned intr);
uint32 system_get_time();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ESPMISSING_H */

