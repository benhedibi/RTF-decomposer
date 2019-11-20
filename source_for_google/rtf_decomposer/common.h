#pragma once

#include "global.h"

typedef uchar t_int_to_hexbuffer[64];
typedef t_int_to_hexbuffer *p_int_to_hexbuffer;

typedef uchar t_int_to_strbuffer[64];
typedef t_int_to_strbuffer *p_int_to_strbuffer;

unsigned_int rtf_strlen(puchar str);
unsigned_int rtf_strlenA(puchar str);
signed_int rtf_itoh(const int value,const p_int_to_hexbuffer conversionbuffer);
puchar rtf_itoa(const int value,const p_int_to_strbuffer conversion_buffer);
puchar rtf_witoa(const int value, const p_int_to_strbuffer conversion_buffer);
boolean rtf_memcmp(puchar src,puchar dest,unsigned_int len);
void rtf_memcpy(const puchar destination,const puchar source,unsigned_int len);
void rtf_memset(const puchar destination,const int value,const unsigned_int len);

