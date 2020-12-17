#ifndef F2C_H
#define F2C_H

#include <stdlib.h>

#define F2C_CHAR_BUFF_SIZE 50

char* _float_to_char(float x, char* p, size_t buff_size);
#define f2c(x,p) _float_to_char(x,p,F2C_CHAR_BUFF_SIZE)
#define nf2c(x,p,n) _float_to_char(x,p,n)

#endif /* F2C_H */
