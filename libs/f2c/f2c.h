#ifndef F2C_H
#define F2C_H

#include <stdlib.h>

#define F2C_CHAR_BUFF_SIZE 15

char* _float_to_char(float x, char* p, size_t buff_size);
#define f2c(val, p_txt) _float_to_char(val, p_txt, F2C_CHAR_BUFF_SIZE)
#define nf2c(val, p_txt, len) _float_to_char(val, p_txt, n)

#endif /* F2C_H */
