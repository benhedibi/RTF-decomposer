#pragma once

#include "global.h"
#include "strpos.h"


typedef struct {
	             unsigned_int decoded_size;
	             uchar buffer[128];
               } t_rtf_decoded_buffer, *p_rtf_decoded_buffer;

typedef struct {
	             puchar data;
	             unsigned_int size;
	             unsigned_int flags;
               } t_rtf_block, *p_rtf_block;

typedef struct {
	             puchar data;
	             unsigned_int size;
	             unsigned_int flags;
	             puchar decoded_data;
	             unsigned_int decoded_size;
	             unsigned_int size_left;
	             unsigned_int decoding_flags;
	             t_rtf_decoded_buffer decoded_buffer;
               } t_rtf_object_data, *p_rtf_object_data;

typedef enum {
	           t_rtf_undefined_state,
	           t_rtf_open_brace_state,
	           t_rtf_close_brace_state,
	           t_rtf_execute_control_state,
	           t_rtf_handle_char_state,
	           t_rtf_asterisk_state
             } t_rtf_state_types;

typedef struct {
	             unsigned_int error_code;
	             puchar data;
	             unsigned_int data_size;
	             unsigned_int hdr_flags;
	             p_str_pos strpos_object;
				 puchar max_parsed_address;
	             puchar overlay_data;
	             unsigned_int overlay_size;
				 unsigned_int malformed_document;
				 t_rtf_state_types current_state;
	             t_rtf_state_types last_state;				 
               } t_rtf_strm, *p_rtf_strm;

boolean open_rtf_strm(const puchar buffer, const unsigned_int buffer_size, const p_rtf_strm strm);
void close_rtf_strm(const p_rtf_strm strm);

void get_document_overlay(const p_rtf_strm strm);
void load_rtf_data(const p_rtf_strm strm);

boolean get_first_ole_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_next_ole_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_ole_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data);

boolean get_first_picture_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_next_picture_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_picture_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data);

boolean get_first_font_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_next_font_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_font_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data);

boolean get_first_datafield_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_next_datafield_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_datafield_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data);

boolean get_first_data_storage_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_next_data_storage_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_data_storage_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data);

boolean get_first_shape_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_next_shape_object(const p_rtf_strm strm, const p_rtf_block object);
boolean get_shape_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data);

boolean read_object_data(const p_rtf_strm strm, const p_rtf_object_data object_data);