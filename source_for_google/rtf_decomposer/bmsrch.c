#include "bmsrch.h"
#include "mallocator.h"
#include "strpos_shared.h"
#include "global.h"
#include "common.h"





typedef struct {
	             p_string string;
	             p_string substring;
	             puchar p_substring;
	             puchar current_buffer_ptr;
	             puchar buffer_limit;
	             p_strpos_memcmp_template memcmp_callback;
	             unsigned_int inc_value;
	             unsigned_int target_string_inc_value;
	             unsigned_int shift_value;
	             uint8 shift_table[256];
               } t_bmoore_srch_params, *p_bmoore_srch_params;


static boolean update_params(const p_bmoore_srch self, const p_string substring, const p_string string)
{
	const t_cmp_string_params cmp_str_callbacks[MAX_t_strpos_srchtype + 1] = { {cmpAString,1,1 },{cmpWString,2,2 },{cmpAWString,1,2 },{ cmpWAString,2,1 } };

	p_bmoore_srch_params params = (p_bmoore_srch_params)((unsigned_int)self + sizeof(*self));
	unsigned_int substring_inc_value, i;
	t_strpos_srchtype srch_type;


	// eviter un integer overflow
	if ((!substring) || (IS_INVALID_STRING(substring)) || (substring->len > sizeof(params->shift_table)) || (!string) || (IS_INVALID_STRING(string)))
	{
		self->error_code = API_ERROR;
		return false;
	}

	params->string = string;
	params->substring = substring;

	params->p_substring = substring->str_direct_ptr;

	if (!params->p_substring)
	{
		params->p_substring = &params->substring->data;
	}

	params->current_buffer_ptr = string->str_direct_ptr;

	if (!params->current_buffer_ptr)
	{
		params->current_buffer_ptr = &string->data;
	}

	params->buffer_limit = params->current_buffer_ptr + string->len;

	if (substring->type == string->type)
	{
		if (substring->type == T_ANSI_STRING_TYPE)
		{
			srch_type = ANSI_SRCH_TYPE;
		}
		else srch_type = UNICODE_SRCH_TYPE;
	}
	else
	{
		if (substring->type == T_ANSI_STRING_TYPE)
		{
			srch_type = ANSI_IN_UNICODE_SRCH_TYPE;
		}
		else srch_type = UNICODE_IN_ANSI_SRCH_TYPE;
	}

	params->memcmp_callback = cmp_str_callbacks[srch_type].callback;

	substring_inc_value = cmp_str_callbacks[srch_type].substring_inc_value;

	params->target_string_inc_value = cmp_str_callbacks[srch_type].string_inc_value;

	if (srch_type == ANSI_IN_UNICODE_SRCH_TYPE)
	{
		params->inc_value = params->substring->len * 2;
	}
	else if (srch_type == UNICODE_IN_ANSI_SRCH_TYPE)
	{
		params->inc_value = params->substring->len / 2;
	}
	else params->inc_value = substring->len;

	if (params->inc_value > sizeof(params->shift_table))
	{
		self->error_code = API_ERROR;
		return false;
	}

	rtf_memset(&params->shift_table, params->inc_value, sizeof(params->shift_table));

	params->inc_value -= params->target_string_inc_value;

	for (i = 0; i < substring->len - substring_inc_value; i += substring_inc_value)
	{
		params->shift_table[params->p_substring[i] | 0x20] = params->inc_value - ((i * params->target_string_inc_value) / substring_inc_value);
	}

	i = (unsigned_int)params->p_substring[substring->len - substring_inc_value] | 0x20;

	params->shift_value = params->shift_table[i];

	params->shift_table[i] = 0;

	params->buffer_limit -= params->inc_value;

	return true;
}

static puchar bm_srch(p_bmoore_srch_params params)
{
	unsigned_int i, buffer_size;


	buffer_size = (unsigned_int)params->buffer_limit - (unsigned_int)params->current_buffer_ptr;

	if ((signed_int)buffer_size < 0) return 0;

	while (true)
	{	

		while (true)
		{
			i = params->shift_table[(unsigned_int)params->current_buffer_ptr[params->inc_value] | 0x20];
			if (!i) break;
			params->current_buffer_ptr += i;
			buffer_size -= i;
			if ((signed_int)buffer_size < 0) return 0;
		}

		if (params->memcmp_callback(params->p_substring, params->current_buffer_ptr, params->substring->len))
		{
			puchar p = params->current_buffer_ptr;
			params->current_buffer_ptr += params->inc_value;
			return p;
		}

		params->current_buffer_ptr += params->shift_value;

		buffer_size -= params->shift_value;

		if ((signed_int)buffer_size < 0) break;
	}

	return 0;
}

static puchar first(const p_bmoore_srch self, const p_string substring, const p_string string)
{

	self->error_code = 0;

	if (!update_params(self, substring, string)) return 0;
	else return bm_srch((p_bmoore_srch_params)self->reserved_data);

}

static puchar next(const p_bmoore_srch self)
{
	p_bmoore_srch_params params = (p_bmoore_srch_params)self->reserved_data;


	if (params->current_buffer_ptr > params->buffer_limit) return 0;
	else return bm_srch(params);

}

p_bmoore_srch new_bmoore_srch(const punsigned_int error_code)
{
  p_bmoore_srch self;


	if (!error_code) return 0;

	*error_code = 0;

	self = (p_bmoore_srch)mem_alloc(sizeof(*self) + sizeof(t_bmoore_srch_params));

	if (!self)
	{
		*error_code = MEM_ALLOC_ERROR;
		return self;
	}

	rtf_memset(self, 0, sizeof(*self) + sizeof(t_bmoore_srch_params));

	self->reserved_data = (opaque_ptr)((unsigned_int)self + sizeof(*self));
	self->reserved_data_size = sizeof(*self) + sizeof(t_bmoore_srch_params);

	self->first = first;
	self->next = next;

	return self;
}

void delete_bmoore_srch(const p_bmoore_srch bmoore_object)
{

	if (bmoore_object)
	{
		mem_free(bmoore_object, sizeof(*bmoore_object));
	}

	return;

}