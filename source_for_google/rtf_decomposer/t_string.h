#pragma once

#include "global.h"

#pragma pack(push,1)

typedef enum {
	           T_UNDEFINED_STRING_TYPE,
	           T_ANSI_STRING_TYPE,
	           T_UNICODE_STRING_TYPE,
	           T_UTF8_STRING_TYPE,
	           T_UTF16_STRING_TYPE,
	           T_UTF32_STRING_TYPE,
	           T_MAX_STRING_TYPE = T_UTF32_STRING_TYPE
             } t_string_types;

typedef struct {
	             unsigned_int len;
	             t_string_types type;
	             unsigned_int utf_size;
	             puchar str_direct_ptr;
	             unsigned_int allocated_size;
	             union {
		                 unsigned_int recursion_level;
		                 unsigned_int reserved;
	                   } u;
	             uchar data[260 * 2];
               } t_string, *p_string; // taille = 512 + 64!

#pragma pack(pop)

#define IS_INVALID_STRING(s) (((s)->type == T_UNDEFINED_STRING_TYPE) || ((s)->type > T_MAX_STRING_TYPE) || ((signed_int)(s)->len <= 0) || ((signed_int)(s)->utf_size < 0) || \
						   (((s)->allocated_size) && (!(s)->str_direct_ptr)) || ((!(s)->str_direct_ptr) && (((s)->len >= sizeof((s)->data)) || ((s)->utf_size >= sizeof((s)->data)))))

