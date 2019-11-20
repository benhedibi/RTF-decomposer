#pragma once

#include "global.h"

typedef enum {
	           ANSI_SRCH_TYPE,
	           UNICODE_SRCH_TYPE,
	           ANSI_IN_UNICODE_SRCH_TYPE,
	           UNICODE_IN_ANSI_SRCH_TYPE,
	           MAX_t_strpos_srchtype = UNICODE_IN_ANSI_SRCH_TYPE
             } t_strpos_srchtype;


typedef boolean t_strpos_memcmp_template(const puchar str1, const puchar str2, unsigned_int len);
typedef t_strpos_memcmp_template *p_strpos_memcmp_template;

typedef puchar t_str_match_template(const opaque_ptr params);
typedef t_str_match_template *p_str_match_template;


typedef struct {
	             p_strpos_memcmp_template callback;
	             unsigned_int substring_inc_value;
	             unsigned_int string_inc_value;
               } t_cmp_string_params, *p_cmp_string_params;

boolean cmpAString(const puchar target_str1, const puchar target_str2,const unsigned_int len);
boolean cmpWString(const puchar target_str1, const puchar target_str2, const unsigned_int len);
boolean cmpWAString(const puchar unicode_str, const const puchar ansi_str, const unsigned_int len);
boolean cmpAWString(const puchar ansi_str, const puchar unicode_str, unsigned_int len);
