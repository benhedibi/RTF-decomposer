#pragma once

#include "global.h"
#include "t_string.h"


#pragma pack(push,1)


typedef struct t_bmoore_srch {
	                           unsigned_int error_code;
	                           opaque_ptr reserved_data;
	                           unsigned_int reserved_data_size;
	                           puchar (*first)(const struct t_bmoore_srch *self, const p_string substring, const p_string string);
	                           puchar (*next)(const struct t_bmoore_srch *self);
                             } *p_bmoore_srch;

#pragma pack(pop)

