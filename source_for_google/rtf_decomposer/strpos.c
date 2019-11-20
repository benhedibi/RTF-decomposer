#include "strpos.h"
#include "strpos_shared.h"
#include "bmsrch.h"
#include "mallocator.h"
#include "common.h"


typedef struct {
	p_string target_buffer;
	p_string substring;
	puchar p_substring;
	puchar p_target_buffer;
	puchar current_buffer_ptr;
	puchar limit_buffer;
	p_bmoore_srch bmoore_srch_object;
	p_strpos_memcmp_template memcmp_callback;
	p_str_match_template match_first_callback;
	p_str_match_template match_next_callback;
	unsigned_int target_string_inc_value;
	unsigned_int substring_match_len;
	t_strpos_strategy strategy;
	struct {
		unsigned_int buffer_mid_pos;
		unsigned_int buffer_end_pos;
		char sub_first_char;
		char sub_mid_char;
		char sub_end_char;
	} quick_srch_params;
} t_str_pos_params, *p_str_pos_params;



static puchar quick_search_f(const p_str_pos_params params)
{
	unsigned_int buffer_size;


	buffer_size = (unsigned_int)(params->limit_buffer - params->current_buffer_ptr);

	if ((signed_int)buffer_size < (signed_int)params->substring->len) return 0;

	while (true)
	{
		if (((params->current_buffer_ptr[0] | 0x20) == params->quick_srch_params.sub_first_char) &&
			((params->current_buffer_ptr[params->quick_srch_params.buffer_mid_pos] | 0x20) == params->quick_srch_params.sub_mid_char) &&
			((params->current_buffer_ptr[params->quick_srch_params.buffer_end_pos] | 0x20) == params->quick_srch_params.sub_end_char) &&
			(params->memcmp_callback(params->p_substring, params->current_buffer_ptr, params->substring->len)))
		{
			puchar p = params->current_buffer_ptr;
			params->current_buffer_ptr += params->substring_match_len;
			return p;
		}

		params->current_buffer_ptr += params->target_string_inc_value;

		buffer_size -= params->target_string_inc_value;

		if ((signed_int)buffer_size <= 0) break;
	}

	return 0;
}

static puchar brute_force(const p_str_pos_params params)
{
	unsigned_int buffer_size;


	buffer_size = (unsigned_int)(params->limit_buffer - params->current_buffer_ptr);

	if (buffer_size < params->substring->len) return 0;

	while (true)
	{
		if (params->memcmp_callback(params->p_substring, params->current_buffer_ptr, params->substring->len))
		{
			puchar p = params->current_buffer_ptr;
			params->current_buffer_ptr += params->substring_match_len;
			return p;
		}

		params->current_buffer_ptr += params->target_string_inc_value;

		buffer_size -= params->target_string_inc_value;

		if ((signed_int)buffer_size <= 0) break;
	}

	return 0;
}

static puchar bmoore_search_first(const p_str_pos_params params)
{
	puchar p;

	p = params->bmoore_srch_object->first(params->bmoore_srch_object, params->substring, params->target_buffer);

	if (p)
	{
		params->current_buffer_ptr = p;
		params->current_buffer_ptr += params->substring->len;
	}

	return p;
}

static puchar bmoore_search_next(const p_str_pos_params params)
{
	puchar p;


	p = params->bmoore_srch_object->next(params->bmoore_srch_object);

	if (p)
	{
		params->current_buffer_ptr = p;
		params->current_buffer_ptr += params->substring->len;
	}

	return p;
}

static boolean update_params(const p_str_pos self, const p_string substring, const p_string string, const t_strpos_strategy strategy)
{
	const t_cmp_string_params cmp_str_callbacks[MAX_t_strpos_srchtype + 1] = { { cmpAString,1,1 },{ cmpWString,2,2 },{cmpAWString,1,2 },{ cmpWAString,2,1 } };

	const p_str_match_template str_match_first_callbacks[MAX_STR_POS_STRATEGY + 1] = { 0,bmoore_search_first,quick_search_f,brute_force };
	const p_str_match_template str_match_next_callbacks[MAX_STR_POS_STRATEGY + 1] = { 0,bmoore_search_next,quick_search_f,brute_force };

	p_str_pos_params params = (p_str_pos_params)self->reserved_data;
	unsigned_int substring_inc_value;
	t_strpos_srchtype srch_type;


	if ((!substring) || (IS_INVALID_STRING(substring)) || (!string) || (IS_INVALID_STRING(string)))
	{
		self->error_code = API_ERROR;
		return 0;
	}

	params->target_buffer = string;
	params->substring = substring;

	params->p_substring = substring->str_direct_ptr;

	if (!params->p_substring)
	{
		params->p_substring = &substring->data;
	}

	params->current_buffer_ptr = string->str_direct_ptr;

	if (!params->current_buffer_ptr)
	{
		params->current_buffer_ptr = &string->data;
	}

	params->limit_buffer = params->current_buffer_ptr + string->len;

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

	params->substring_match_len = (params->substring->len / substring_inc_value) * params->target_string_inc_value;

	params->strategy = strategy;

	if (strategy == ADAPTATIVE_STRATEGY)
	{
		if (string->len / params->target_string_inc_value < 64)
		{
			if (substring->len / substring_inc_value < 4)
			{
				params->strategy = BRUTE_FORCE_STRATEGY;
			}
			else params->strategy = QUICK_SEARCH_STRATEGY;
		}
		else
		{
			if (substring->len / substring_inc_value < 4)
			{
				params->strategy = BRUTE_FORCE_STRATEGY;
			}
			else if (substring->len / substring_inc_value < 6)
			{
				params->strategy = QUICK_SEARCH_STRATEGY;
			}
			else params->strategy = BOYER_MOORE_STRATEGY;
		}
	}

	if ((params->strategy == BOYER_MOORE_STRATEGY) && (!params->bmoore_srch_object))
	{
		params->bmoore_srch_object = new_bmoore_srch(&self->error_code);
		if (!params->bmoore_srch_object) return false;
	}

	if (params->strategy == QUICK_SEARCH_STRATEGY)
	{
		params->quick_srch_params.buffer_end_pos = params->substring_match_len - params->target_string_inc_value;
		params->quick_srch_params.buffer_mid_pos = params->quick_srch_params.buffer_end_pos / 2;
		params->quick_srch_params.sub_first_char = params->p_substring[0] | 0x20;
		params->quick_srch_params.sub_mid_char = params->p_substring[(params->substring->len - substring_inc_value) / 2] | 0x20;
		params->quick_srch_params.sub_end_char = params->p_substring[params->substring->len - substring_inc_value] | 0x20;
	}

	params->match_first_callback = str_match_first_callbacks[params->strategy];
	params->match_next_callback = str_match_next_callbacks[params->strategy];

	return true;
}

static puchar first(const p_str_pos self, const p_string substring, const p_string string, const t_strpos_strategy strategy)
{

	self->error_code = 0;

	if (!update_params(self, substring, string, strategy)) return 0;

	return ((p_str_pos_params)self->reserved_data)->match_first_callback((p_str_pos_params)self->reserved_data);

}

static puchar next(const p_str_pos self)
{
	p_str_pos_params params = (p_str_pos_params)self->reserved_data;


	if (params->current_buffer_ptr >= params->limit_buffer) return 0;
	else return params->match_next_callback(params);

}

p_str_pos new_str_pos(const punsigned_int error_code)
{
	p_str_pos self;


	if (!error_code) return 0;

	self = mem_alloc(sizeof(*self) + sizeof(t_str_pos_params));

	if (!self)
	{
		*error_code = MEM_ALLOC_ERROR;
		return 0;
	}

	rtf_memset(self, 0, sizeof(*self) + sizeof(t_str_pos_params));

	self->reserved_data = (opaque_ptr)((unsigned_int)self + sizeof(*self));
	self->reserved_data_size = sizeof(t_str_pos_params);

	self->first = first;
	self->next = next;

	return self;
}

void delete_str_pos(const p_str_pos str_pos_object)
{

	if (str_pos_object)
	{
		p_str_pos_params params = (p_str_pos_params)str_pos_object->reserved_data;

		if (params->bmoore_srch_object)
		{
			delete_bmoore_srch(params->bmoore_srch_object);
		}

		mem_free(str_pos_object, sizeof(*str_pos_object) + sizeof(t_str_pos_params));
	}

	return;
}