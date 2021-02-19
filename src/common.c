/**
 * \file common.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-02-18
 */

#include <osapi.h>
#include <mem.h>

#include <common.h>


char* ICACHE_FLASH_ATTR
common_strdup(const char* str) {
    char* new_str = NULL;

	if (str == NULL) {
		return NULL;
	}

	new_str = (char*)os_malloc(sizeof(char) * (os_strlen(str) + 1));

	if (new_str == NULL) {
		return NULL;
	}

	os_strcpy(new_str, str);
	return new_str;
}
