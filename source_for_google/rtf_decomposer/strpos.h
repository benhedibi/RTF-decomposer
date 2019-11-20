#pragma once

#include "global.h"
#include "t_string.h"

#pragma pack(push,1)

typedef enum {
	           ADAPTATIVE_STRATEGY,
	           BOYER_MOORE_STRATEGY,
	           QUICK_SEARCH_STRATEGY,
	           BRUTE_FORCE_STRATEGY,
	           MAX_STR_POS_STRATEGY = BRUTE_FORCE_STRATEGY
             } t_strpos_strategy;

typedef struct t_str_pos {
	                       unsigned_int error_code;
	                       opaque_ptr reserved_data;
	                       unsigned_int reserved_data_size;
	                       puchar (*first)(const struct t_str_pos*, const p_string substring, const p_string string, const t_strpos_strategy strategy);
	                       puchar (*next)(const struct t_str_pos*);
                         } *p_str_pos;

#pragma pack(pop)

p_str_pos new_str_pos(const punsigned_int error_code);
void delete_str_pos(const p_str_pos str_pos_object);
