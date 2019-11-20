#pragma once

#include "global.h"
#include "t_string.h"

typedef enum {
	           t_io_undefined_object,
			   t_io_file_object,
	           t_io_folder_object
             } t_io_object_types;

typedef boolean t_read_file_callback(const opaque_ptr user_data, const p_string file_path);
typedef t_read_file_callback *p_read_file_callback;

typedef struct {
	             opaque_ptr user_data;
				 p_read_file_callback read_callback;
				 unsigned_int rec_level;
               } t_read_dir_params,*p_read_dir_params;

typedef struct { 
                 puchar path;
				 unsigned_int path_len;
				 puchar file_name;
				 unsigned_int file_name_len;
				 unsigned_int size_left;
               } t_io_path_infos,*p_io_path_infos;


unsigned_int open_file(const puchar file_path);
unsigned_int create_file(const puchar file_path);
void close_file(const unsigned_int file_handle);
int64 get_file_size(const unsigned_int file_handle);
boolean read_file(const unsigned_int file_handle,const puchar read_buffer,const unsigned_int read_size);
boolean write_file(const unsigned_int file_handle, const puchar write_buffer, const unsigned_int write_size);
t_io_object_types get_io_type(const puchar path);
unsigned_int read_dir(const p_string dir_path, const p_read_dir_params params);
boolean crack_io_path(const p_string path,const p_io_path_infos io_path_infos);


typedef enum {
	           t_undefined_console_text_mode,
			   t_infos_console_text_mode,
			   t_warning_console_text_mode,
			   t_error_console_text_mode
             } t_console_text_modes;


#define CONSOLE_DEFAULT_COLOR (7)

unsigned_int open_console(const boolean input_mode);
void close_console(const unsigned_int console_handle);
boolean write_console(const unsigned_int console_handle, const puchar msg, const unsigned_int msg_size,const t_console_text_modes text_mode);
boolean inline_console(const unsigned_int console_handle, const puchar line, const unsigned_int line_len,COORD *coordinates,const t_console_text_modes text_mode);

