
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "io.h"
#include "mallocator.h"
#include "rtf_lib.h"
#include "common.h"
#include <shellapi.h>


typedef struct {
	             unsigned_int argc;
	             puchar argv[16];
               } t_argument_list, *p_argument_list;



#define RTF_DECOMPOSER_VERSION_STRING "RTF Decomposer 1.0"

typedef enum {
               t_undefined_rtf_dump,
			   t_rtf_ole_dump,
			   t_rtf_picture_dump,
			   t_rtf_font_dump,
			   t_rtf_macro_dump,
			   t_rtf_datastorage_dump,
			   t_rtf_overlay_dump,
			   t_rtf_datafield_dump,
			   t_rtf_shape_dump,
			   RTF_MAX_DUMP_TYPES = t_rtf_shape_dump
             } t_rtf_data_dump_types;


#pragma pack(push,1)

typedef struct { 
	             unsigned_int console_handle;
				 unsigned_int input_handle;
				 t_string source_path;
				 t_string destination_path;
				 unsigned_int errors;
				 unsigned_int log_handle;
				 puchar pack_buff;
				 unsigned_int pack_buff_size;
				 puchar unpack_buff;
				 unsigned_int unpack_buff_size;
				 p_rtf_strm rtf_strm;
				 t_io_object_types src_object_type;
				 t_io_object_types dest_object_type;
				 unsigned_int files_count;
				 unsigned_int objects_count;
				 CONSOLE_SCREEN_BUFFER_INFO screen_coordinates;
				 boolean extract_ole;
	             boolean extract_pictures;
	             boolean extract_font;
	             boolean extract_datafield;
	             boolean extract_datastore;
	             boolean extract_overlays;
				 boolean extract_shapes;
				 boolean operation_cancelled;				 
               } t_rtf_options,*p_rtf_options;

#pragma pack(pop)


static void report_error(const unsigned_int console_handle, const unsigned_int error_code)
{
  puchar msg = NULL;
  boolean allocated_buffer = false;


	switch (error_code)
	{
	        case ERROR_SUCCESS : return;

	             case IO_ERROR : msg = "An IO Error has occured !";
		                         break;

	            case API_ERROR : msg = "An unexepected API error has occured !";
		                         break;
	  
	  case INVALID_PARAM_ERROR : msg = "The specified option switch is invalid.";
		                         break;

	  case PATH_TOO_LONG_ERROR : msg = "File path is too long.";
	                             break;

        case SOURCE_FILE_ERROR : msg = "The specified source is unacessible or invalid.";
		                         break;

      case TARGET_FOLDER_ERROR : msg = "The specified target folder is unaccessible or invalid.";
	                             break;

	                   default : FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, 0, &msg, 0, NULL);
		                         allocated_buffer = true;
		                         break;
	}

	if ((signed_int)console_handle <= 0)
	{
		MessageBoxA(NULL, msg, RTF_DECOMPOSER_VERSION_STRING, MB_ICONERROR);
	}
	else write_console(console_handle, msg, rtf_strlenA(msg), t_error_console_text_mode);

	if ((allocated_buffer) && (msg))
	{
		LocalFree(msg);
	}

	return;
}

static void show_help(const unsigned_int console_handle)
{
  puchar t1 = "**********************************************************************\n";
  puchar t2 = "*                                                                    *\n";
  puchar t3 = "*                          RTF Decomposer 1.0                        *\n";
  puchar t4 = "*          Ben Hedibi Hassene - Tornado Security Labs, Canada.       *\n";
  puchar t5 = "*                                                                    *\n";
  puchar t6 = "*                                                                    *\n";
  puchar t7 = "**********************************************************************\n\n";
  puchar usage = "usage : unrtf.exe {options} {file} or {folder}\n\n";
  puchar example = "examples :\n";
  puchar example1 = "- extract OLE embedded data : unrtf.exe -o file.rtf\n";
  puchar example2 = "- extract all : unrtf.exe -a -t file.rtf c:\\output_folder \n";
  puchar example3 = "- RTF bulk extraction : unrtf.exe -a -t e:\\rtf_repo c:\\out_folder \n\n";
  puchar options0 = "options :\n";
  puchar options1 = "-o : extract OLE1 data only \n";
  puchar options2 = "-a : extract all embedded data types \n";
  puchar options3 = "-e : extract RTF overlay data\n";
  puchar options4 = "-l : log errors\n";
  puchar options5 = "-t : specify an output folder path\n";
	
    
	
	if (!write_console(console_handle, t1, rtf_strlenA(t1), t_infos_console_text_mode))
    {
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, t2, rtf_strlenA(t2), t_infos_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, t3, rtf_strlenA(t3), t_infos_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, t4, rtf_strlenA(t4), t_infos_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, t5, rtf_strlenA(t5), t_infos_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, t6, rtf_strlenA(t6), t_infos_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}
	
	if (!write_console(console_handle, t7, rtf_strlenA(t7), t_infos_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle,usage, rtf_strlenA(usage), t_undefined_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, example, rtf_strlenA(example), t_warning_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, example1, rtf_strlenA(example1), t_undefined_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, example2, rtf_strlenA(example2), t_undefined_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, example3, rtf_strlenA(example3), t_undefined_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}
	if (!write_console(console_handle, options0, rtf_strlenA(options0), t_warning_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, options1, rtf_strlenA(options1), t_undefined_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, options2, rtf_strlenA(options2), t_undefined_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, options3, rtf_strlenA(options3), t_undefined_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, options4, rtf_strlenA(options4), t_undefined_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	if (!write_console(console_handle, options5, rtf_strlenA(options5), t_undefined_console_text_mode))
	{
		report_error(console_handle, GetLastError());
		return;
	}

	return;
}

static void report_operation_error(const p_rtf_options rtf_options, const puchar file_path, const unsigned_int error_code, const PWSTR custom_msg)
{
  PWSTR msg = NULL;


	if (custom_msg)
	{
		if (!write_file(rtf_options->log_handle, custom_msg, rtf_strlen(custom_msg)))
		{
			report_error(rtf_options->log_handle, IO_ERROR);
		}

		return;
	}

	switch (error_code)
	{
	case IO_ERROR: msg = "IO error has occured with :\n";
		break;

	case RTF_INVALID_SIGNATURE_ERROR: msg = "Not a valid RTF file :\n";
		break;

	case BUFFER_TOO_SMALL_ERROR: msg = "Buffer overrun with :\n";
		break;

	case FILE_TOO_BIG_ERROR: msg = "File too big :\n";
		break;

	case UNPACK_ERROR: msg = "Data extraction error with :\n";
		break;

	case EMPTY_FILE_ERROR: msg = "Empty file :\n";
		break;

	default: msg = "Unspecified error has occured :\n";
		break;
	}

	if ((!write_file(rtf_options->log_handle, msg, rtf_strlen(msg))) || (!write_file(rtf_options->log_handle, file_path, rtf_strlen(file_path))) ||
		(!write_file(rtf_options->log_handle," \n", 4)))
	{
		report_error(rtf_options->log_handle, IO_ERROR);
	}

	return;
}

static void write_progress_status(const p_rtf_options rtf_options)
{
  const puchar processed_files = "Processed files ";
  const puchar unpacked_objects = " | Extracted objects ";
  const puchar errors = " | Errors ";

  unsigned_int status_len, len;
  puchar p; 
  t_int_to_strbuffer conversion_buffer;
  uchar status[128];


	status_len = 16;

	rtf_memcpy(&status, processed_files, status_len);

	p = rtf_itoa(rtf_options->files_count, &conversion_buffer);

	len = rtf_strlenA(p);

	if (((signed_int)len <= 0) || (status_len + len + 21 + 10 >= sizeof(status)))
	{
	   report_error(rtf_options->console_handle,API_ERROR);
	   return;
	}

	rtf_memcpy(&status[status_len], p, len);

	status_len += len;
	
	len = 21;

	rtf_memcpy(&status[status_len], unpacked_objects, len);

	status_len += len;
		
	p = rtf_itoa(rtf_options->objects_count, &conversion_buffer);

	len = rtf_strlenA(p);

	if (((signed_int)len <= 0) || (status_len + len + 10 >= sizeof(status)))
	{
		report_error(rtf_options->console_handle, API_ERROR);
		return;
	}

	rtf_memcpy(&status[status_len], p, len);

	status_len += len;

	len = 10;

	rtf_memcpy(&status[status_len], errors, len);

	status_len += len;

	p = rtf_itoa(rtf_options->errors, &conversion_buffer);

	len = rtf_strlenA(p);

	if (((signed_int)len <= 0) || (status_len + len >= sizeof(status)))
	{
		report_error(rtf_options->console_handle, API_ERROR);
		return;
	}

	rtf_memcpy(&status[status_len], p, len);

	status_len += len;

	status[status_len] = 0;
		
	inline_console(rtf_options->console_handle, &status, status_len, &rtf_options->screen_coordinates.dwCursorPosition, t_undefined_console_text_mode);

	return;
}

static boolean dump_data(const p_rtf_options rtf_options,const puchar buffer,const unsigned_int buffer_size,const p_string src_path,const t_rtf_data_dump_types dump_type,const int index)
{
  const puchar dump_names[RTF_MAX_DUMP_TYPES + 1] = {NULL,L"_OLE",L"_PIC",L"_FNT",L"_MAC",L"_STG",L"_OVL",L"_DFL",L"_SHP"};

  puchar p;
  unsigned_int num_len,fd;
  t_io_path_infos io_path_infos; 
  t_int_to_strbuffer conv_buff;
  t_string path_result;  

  
    if ((dump_type == t_undefined_rtf_dump) || (dump_type > RTF_MAX_DUMP_TYPES) || ((signed_int)index < 0))
    {
	  report_error(rtf_options->console_handle, API_ERROR);
	  return false;
    }

	num_len = 0;

	p = NULL;

	if (index)
	{
	  p = rtf_witoa(index,&conv_buff);
	  num_len = rtf_strlen(p); 
	}
	
	if (!crack_io_path(src_path,&io_path_infos))
	{
	  report_error(rtf_options->console_handle,GetLastError());
	  return false;
	}
	
	if (rtf_options->destination_path.len + io_path_infos.file_name_len + num_len + 8 >= sizeof(path_result.data))
	{
	  report_error(rtf_options->console_handle,PATH_TOO_LONG_ERROR);
	  return false;
	}
	
	rtf_memcpy(&path_result, &rtf_options->destination_path, sizeof(rtf_options->destination_path));
	
	rtf_memcpy(&path_result.data[path_result.len], io_path_infos.file_name,io_path_infos.file_name_len);

	path_result.len += io_path_infos.file_name_len;
	
	*(puint64)&path_result.data[path_result.len] = *(puint64)dump_names[dump_type];
	
	path_result.len += 8;

    if (p)
	{
	  rtf_memcpy(&path_result.data[path_result.len],p,num_len);
	  path_result.len += num_len;
	}

	path_result.data[path_result.len] = 0;

	fd = create_file(&path_result.data);

	if (fd == INVALID_HANDLE_VALUE)
	{
      report_error(rtf_options->console_handle,GetLastError());  
	  return false;
	}

	if (!write_file(fd,buffer,buffer_size))
	{
	  report_error(rtf_options->console_handle,IO_ERROR);
	  close_file(fd);
	  return false;
	}

	rtf_options->objects_count++;
	
	write_progress_status(rtf_options);
	
	return true;
}

static void extract_ole_objects(const p_rtf_options rtf_options,const p_string src_path)
{
  unsigned_int limit = (unsigned_int)rtf_options->rtf_strm->data + rtf_options->rtf_strm->data_size;
  unsigned_int index;
  t_rtf_block rtf_block;
  t_rtf_object_data rtf_object_data;


	if (!get_first_ole_object(rtf_options->rtf_strm, &rtf_block))
	{
	  if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
	  {
	    if (rtf_options->log_handle)
	    {
		  report_operation_error(rtf_options,&src_path->data, UNPACK_ERROR, NULL);
	    }

	    rtf_options->errors++;
	  }

	  return;	  	
	}

	index = 0;

    while (true)
	{
	  if (!get_ole_object_data(rtf_options->rtf_strm, &rtf_block, &rtf_object_data))
	  {
		if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
		{
		  if (rtf_options->log_handle)
		  {
			report_operation_error(rtf_options,&src_path->data, UNPACK_ERROR, NULL);
		  }

		  rtf_options->errors++;
	    }

	    goto next;
	  }

	  rtf_object_data.decoded_data = rtf_options->unpack_buff;
	  rtf_object_data.decoded_size = 0;
	  rtf_object_data.size_left = rtf_options->unpack_buff_size;
	  rtf_object_data.decoding_flags = 0;

      if (!read_object_data(rtf_options->rtf_strm, &rtf_object_data))
	  {
		if (rtf_options->log_handle)
		{
		  report_operation_error(rtf_options,&src_path->data, UNPACK_ERROR, NULL);
	    }

		rtf_options->errors++;

	    rtf_block.data = rtf_object_data.data;
		rtf_block.size = limit - (unsigned_int)rtf_block.data;
	  }
	  else 
	  {
	    if (!dump_data(rtf_options, rtf_options->unpack_buff,rtf_object_data.decoded_size,src_path,t_rtf_ole_dump,index++))
		{
		  if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
		  {
			if (rtf_options->log_handle)
			{
			  report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
		    }
			
			rtf_options->errors++;
		  }
		}		
	  }

	  next:

	  if (!get_next_ole_object(rtf_options->rtf_strm, &rtf_block)) break;
	}

	if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
	{
	  if (rtf_options->log_handle)
	  {
		report_operation_error(rtf_options,&src_path->data, UNPACK_ERROR, NULL);
   	  }

	  rtf_options->errors++;
	}
	
	return;
}
 
static void extract_picture_objects(const p_rtf_options rtf_options, const p_string src_path)
{
  unsigned_int limit = (unsigned_int)rtf_options->rtf_strm->data + rtf_options->rtf_strm->data_size;
  unsigned_int index;
  t_rtf_block rtf_block;
  t_rtf_object_data rtf_object_data;


	if (!get_first_picture_object(rtf_options->rtf_strm, &rtf_block))
	{
	  if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
	  {
		if (rtf_options->log_handle)
		{
		  report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
		}

		rtf_options->errors++;
	  }
	  
	  return; 	
	}

	index = 0;

	while (true)
	{
		if (!get_picture_object_data(rtf_options->rtf_strm, &rtf_block, &rtf_object_data))
		{
			if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}

			goto next;
		}

		rtf_object_data.decoded_data = rtf_options->unpack_buff;
		rtf_object_data.decoded_size = 0;
		rtf_object_data.size_left = rtf_options->rtf_strm->data_size;
		rtf_object_data.decoding_flags = 0;

		if (!read_object_data(rtf_options->rtf_strm, &rtf_object_data))
		{
			if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}

			rtf_block.data = rtf_object_data.data;
			rtf_block.size = limit - (unsigned_int)rtf_block.data;
		}
		else
		{
			if (!dump_data(rtf_options,rtf_options->unpack_buff, rtf_object_data.decoded_size, src_path,t_rtf_picture_dump, index++))
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}			
		}

	next:

		if (!get_next_picture_object(rtf_options->rtf_strm, &rtf_block)) break;
	}

	if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
	{
		if (rtf_options->log_handle)
		{
			report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
		}

		rtf_options->errors++;
	}

	return;
}

static void extract_font_objects(const p_rtf_options rtf_options, const p_string src_path)
{
  unsigned_int limit = (unsigned_int)rtf_options->rtf_strm->data + rtf_options->rtf_strm->data_size;
  unsigned_int index;
  t_rtf_block rtf_block;
  t_rtf_object_data rtf_object_data;


	
	if (!get_first_font_object(rtf_options->rtf_strm, &rtf_block))
	{
		if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
		{
			if (rtf_options->log_handle)
			{
				report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
			}

			rtf_options->errors++;
		}

		return;
	}

	index = 0;

	while (true)
	{
		if (!get_font_object_data(rtf_options->rtf_strm, &rtf_block, &rtf_object_data))
		{
			if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}

			goto next;
		}

		rtf_object_data.decoded_data = rtf_options->unpack_buff;
		rtf_object_data.decoded_size = 0;
		rtf_object_data.size_left = rtf_options->rtf_strm->data_size;
		rtf_object_data.decoding_flags = 0;

		if (!read_object_data(rtf_options->rtf_strm, &rtf_object_data))
		{
			if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}

			rtf_block.data = rtf_object_data.data;
			rtf_block.size = limit - (unsigned_int)rtf_block.data;
		}
		else
		{
			if (!dump_data(rtf_options,rtf_options->unpack_buff, rtf_object_data.decoded_size, src_path,t_rtf_font_dump, index++))
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}			
		}

	next:

		if (!get_next_font_object(rtf_options->rtf_strm, &rtf_block)) break;
	}

	if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
	{
		if (rtf_options->log_handle)
		{
			report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
		}

		rtf_options->errors++;
	}

	return;
}

static void extract_datafield_objects(const p_rtf_options rtf_options, const p_string src_path)
{
  unsigned_int limit = (unsigned_int)rtf_options->rtf_strm->data + rtf_options->rtf_strm->data_size;
  unsigned_int index;
  t_rtf_block rtf_block;
  t_rtf_object_data rtf_object_data;


	
	if (!get_first_datafield_object(rtf_options->rtf_strm, &rtf_block))
	{
		if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
		{
			if (rtf_options->log_handle)
			{
				report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
			}

			rtf_options->errors++;
		}

		return;
	}

	index = 0;

	while (true)
	{
		if (!get_datafield_object_data(rtf_options->rtf_strm, &rtf_block, &rtf_object_data))
		{
			if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}

			goto next;
		}

		rtf_object_data.decoded_data = rtf_options->unpack_buff;
		rtf_object_data.decoded_size = 0;
		rtf_object_data.size_left = rtf_options->rtf_strm->data_size;
		rtf_object_data.decoding_flags = 0;

		if (!read_object_data(rtf_options->rtf_strm, &rtf_object_data))
		{
			if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}

			rtf_block.data = rtf_object_data.data;
			rtf_block.size = limit - (unsigned_int)rtf_block.data;
		}
		else
		{
			if (!dump_data(rtf_options,rtf_options->unpack_buff, rtf_object_data.decoded_size, src_path, t_rtf_datafield_dump, index++))
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}			
		}

	next:

		if (!get_next_datafield_object(rtf_options->rtf_strm, &rtf_block)) break;
	}

	if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
	{
		if (rtf_options->log_handle)
		{
			report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
		}

		rtf_options->errors++;
	}

	return;
}

static void extract_data_storage_objects(const p_rtf_options rtf_options, const p_string src_path)
{
  unsigned_int limit = (unsigned_int)rtf_options->rtf_strm->data + rtf_options->rtf_strm->data_size;
  unsigned_int index;
  t_rtf_block rtf_block;
  t_rtf_object_data rtf_object_data;


	if (!get_first_data_storage_object(rtf_options->rtf_strm, &rtf_block))
	{
		if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
		{
			if (rtf_options->log_handle)
			{
				report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
			}

			rtf_options->errors++;
		}

		return;
	}

	index = 0;

	while (true)
	{
		if (!get_data_storage_object_data(rtf_options->rtf_strm, &rtf_block, &rtf_object_data))
		{
			if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}

			goto next;
		}

		rtf_object_data.decoded_data = rtf_options->unpack_buff;
		rtf_object_data.decoded_size = 0;
		rtf_object_data.size_left = rtf_options->rtf_strm->data_size;
		rtf_object_data.decoding_flags = 0;

		if (!read_object_data(rtf_options->rtf_strm, &rtf_object_data))
		{
			if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}

			rtf_block.data = rtf_object_data.data;
			rtf_block.size = limit - (unsigned_int)rtf_block.data;
		}
		else
		{
			if (!dump_data(rtf_options,rtf_options->unpack_buff, rtf_object_data.decoded_size, src_path, t_rtf_datastorage_dump, index++))
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}			
		}

	next:

		if (!get_next_data_storage_object(rtf_options->rtf_strm, &rtf_block)) break;
	}

	if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
	{
		if (rtf_options->log_handle)
		{
			report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
		}

		rtf_options->errors++;
	}

	return;
}

static void extract_shape_objects(const p_rtf_options rtf_options, const p_string src_path)
{
	unsigned_int limit = (unsigned_int)rtf_options->rtf_strm->data + rtf_options->rtf_strm->data_size;
	unsigned_int index;
	t_rtf_block rtf_block;
	t_rtf_object_data rtf_object_data;


	if (!get_first_shape_object(rtf_options->rtf_strm, &rtf_block))
	{
		if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
		{
			if (rtf_options->log_handle)
			{
				report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
			}

			rtf_options->errors++;
		}

		return;
	}

	index = 0;

	while (true)
	{
		if (!get_shape_object_data(rtf_options->rtf_strm, &rtf_block, &rtf_object_data))
		{
			if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}

			goto next;
		}

		rtf_object_data.decoded_data = rtf_options->unpack_buff;
		rtf_object_data.decoded_size = 0;
		rtf_object_data.size_left = rtf_options->rtf_strm->data_size;
		rtf_object_data.decoding_flags = 0;

		if (!read_object_data(rtf_options->rtf_strm, &rtf_object_data))
		{
			if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}

			rtf_block.data = rtf_object_data.data;
			rtf_block.size = limit - (unsigned_int)rtf_block.data;
		}
		else
		{
			if (!dump_data(rtf_options, rtf_options->unpack_buff, rtf_object_data.decoded_size, src_path, t_rtf_shape_dump, index++))
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
				}

				rtf_options->errors++;
			}			
		}

	next:

		if (!get_next_shape_object(rtf_options->rtf_strm, &rtf_block)) break;
	}

	if (rtf_options->rtf_strm->error_code != ERROR_SUCCESS)
	{
		if (rtf_options->log_handle)
		{
			report_operation_error(rtf_options, &src_path->data, UNPACK_ERROR, NULL);
		}

		rtf_options->errors++;
	}

	return;
}

static boolean is_operations_cancelled(const p_rtf_options rtf_options)
{
  unsigned_int count;
  INPUT_RECORD input;
     
	 count = 0;

	 if (!GetNumberOfConsoleInputEvents(rtf_options->input_handle,&count))
	 {
	   report_error(rtf_options->console_handle,GetLastError());
	   return false;
	 }

	 if (!count) return false;

	 if (!PeekConsoleInput(rtf_options->input_handle,&input,1,&count))
	 {
	   report_error(rtf_options->console_handle, GetLastError());
	   return false;
	 }

	 return ((input.EventType == KEY_EVENT) && (input.Event.KeyEvent.bKeyDown) && (input.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE));
	 
}

static boolean unpack_rtf_object(const p_rtf_options rtf_options,const p_string file_path)
{   
  unsigned_int fd;
  int64 fsize;
    	
	
	rtf_options->files_count++;
	  
    write_progress_status(rtf_options);
	
	fd = open_file(&file_path->data);

	if (fd == INVALID_HANDLE_VALUE)
	{
	  if (rtf_options->log_handle)
	  {
	    report_operation_error(rtf_options,&file_path->data,IO_ERROR,NULL);
	  }
	  
	  rtf_options->errors++;

	  return true;
	}

	fsize = get_file_size(fd);

	if (fsize <= 0)
	{
	  if (rtf_options->log_handle)
	  {
		report_operation_error(rtf_options, &file_path->data, EMPTY_FILE_ERROR, NULL);
	  }

	  rtf_options->errors++;	  
	  CloseHandle(fd);

	  return true;
	}

	if (fsize >= MAXDWORD32)
	{
	  if (rtf_options->log_handle)
	  {
		report_operation_error(rtf_options, &file_path->data, FILE_TOO_BIG_ERROR, NULL);
	  }
	  
	  rtf_options->errors++;
	  
	  CloseHandle(fd);

	  return true;	
	}

	if ((unsigned_int)fsize > rtf_options->pack_buff_size)
	{
	  	  
	  if (rtf_options->pack_buff)
	  {
	    mem_free(rtf_options->pack_buff,rtf_options->pack_buff_size);
		mem_free(rtf_options->unpack_buff,rtf_options->unpack_buff_size);
      }
	  	  
	  rtf_options->pack_buff = mem_alloc((unsigned_int)fsize);
	  
	  if (!rtf_options->pack_buff)
	  {        
		CloseHandle(fd);
		report_error(rtf_options->console_handle, MEM_ALLOC_ERROR);
		return false;	  
	  }

	  rtf_options->unpack_buff_size = (unsigned_int)fsize;

	  rtf_options->unpack_buff = mem_alloc((unsigned_int)fsize);
      
	  if (!rtf_options->unpack_buff)
	  {
		CloseHandle(fd);
		report_error(rtf_options->console_handle, MEM_ALLOC_ERROR);
		return false;
	  }

	  rtf_options->pack_buff_size = rtf_options->unpack_buff_size = (unsigned_int)fsize;	  
	}
	
	rtf_options->rtf_strm->data = rtf_options->pack_buff;
	rtf_options->rtf_strm->data_size = (unsigned_int)fsize;

	if (!read_file(fd, rtf_options->rtf_strm->data,(unsigned_int)fsize))
	{
	  if (rtf_options->log_handle)
	  {
		report_operation_error(rtf_options, &file_path->data, IO_ERROR, NULL);
	  }

	  rtf_options->errors++;

	  CloseHandle(fd);

	  return true;
	}

	CloseHandle(fd);

	if (!open_rtf_strm(rtf_options->rtf_strm->data,rtf_options->rtf_strm->data_size,rtf_options->rtf_strm))
	{
	  if (rtf_options->log_handle)
	  {
	    report_operation_error(rtf_options,&file_path->data,rtf_options->rtf_strm->error_code,NULL);
	  }
	  
	  rtf_options->errors++;

	  return true;
	}
	
	if (rtf_options->extract_ole)
	{	  
	  extract_ole_objects(rtf_options,file_path);
	}

	if (rtf_options->extract_pictures)
	{
	  extract_picture_objects(rtf_options,file_path);
	}

	if (rtf_options->extract_font)
	{
	  extract_font_objects(rtf_options,file_path);
	}

	if (rtf_options->extract_datafield)
	{
	  extract_datafield_objects(rtf_options,file_path);
	}

	if (rtf_options->extract_datastore)
	{
	  extract_data_storage_objects(rtf_options,file_path);
	}
	
	/*if (rtf_options->extract_shapes)
	{
	  extract_shape_objects(rtf_options, file_path);
	}*/

	if (rtf_options->extract_overlays)
	{
		get_document_overlay(rtf_options->rtf_strm);

		if ((rtf_options->rtf_strm->overlay_data) && ((signed_int)rtf_options->rtf_strm->overlay_size > 0))
		{
			if (!dump_data(rtf_options, rtf_options->rtf_strm->overlay_data, rtf_options->rtf_strm->overlay_size, file_path, t_rtf_overlay_dump, 0))
			{
				if (rtf_options->log_handle)
				{
					report_operation_error(rtf_options, &file_path->data, GetLastError(), NULL);
				}
				rtf_options->errors++;
			}			
		}
	}

	//write_progress_status(rtf_options);

	return true;
}

void unpack_rtf(const p_rtf_options rtf_options)
{  
  t_rtf_strm rtf_strm;
     				

	rtf_memset(&rtf_strm,0,sizeof(rtf_strm));
	
	rtf_options->rtf_strm = &rtf_strm;
    
	rtf_options->source_path.type = rtf_options->destination_path.type = T_UNICODE_STRING_TYPE;

	if (rtf_options->dest_object_type == t_io_folder_object)
	{
		if (rtf_options->destination_path.data[rtf_options->destination_path.len - 2] != '\\')
		{
			rtf_options->destination_path.data[rtf_options->destination_path.len++] = '\\';
			rtf_options->destination_path.data[rtf_options->destination_path.len++] = 0;
		}
	}

	if (rtf_options->src_object_type == t_io_folder_object)
	{
      t_read_dir_params params;
	  	    
		params.read_callback = unpack_rtf_object;
		params.user_data = rtf_options;
		params.rec_level = 0;

		read_dir(&rtf_options->source_path, &params);
	}
	else 
	{		
	  unpack_rtf_object(rtf_options,&rtf_options->source_path);
	}	
	
	/*close_console(rtf_options->input_handle);

	rtf_options->input_handle = 0;*/

	if (rtf_options->unpack_buff)
	{
	  mem_free(rtf_options->unpack_buff,rtf_options->unpack_buff_size);
	}
	
	if (rtf_options->pack_buff)
	{
	  mem_free(rtf_options->pack_buff,rtf_options->pack_buff_size);
	}	
	
	return;
}

boolean convert_argument_list(const LPWSTR *arguments,const p_argument_list arg_list,int count)
{
  int i;
  PWCHAR src;
  puchar dest;
  unsigned_int len;
    
	
	SetLastError(ERROR_SUCCESS);

	arg_list->argc = 0;
	
	for (int i = 0; i < count; i++)
	{	  	  
	  src = arguments[i];
	  
	  if (!src) 
	  {
	    SetLastError(API_ERROR);
		break;
      }

	  len = 0;

	  while (true)
	  {
	    if (*src++ == 0) break;
		len++;
	  }

	  if (!len) 
	  {
	    SetLastError(API_ERROR);
	   break;
      }

	  dest = mem_alloc(len + 1);
	  
	  if (!dest) 
	  {
	    SetLastError(MEM_ALLOC_ERROR);
	    break;
	  }

	  arg_list->argv[i] = dest;
	  
	  src = arguments[i];

	  while (true)
      {
	    *dest++ = (puchar)*src++;
		len--;
		if (!len) break;
	  }	  
	  
	  *dest = 0;
	 
	  arg_list->argc++;	  
	}

	return ((GetLastError() == ERROR_SUCCESS) && (count == arg_list->argc));
}

void free_argument_list(const p_argument_list arg_list)
{
  unsigned_int i,len;
     

    for (i = 0; i < arg_list->argc; i++)
    {
      if (arg_list->argv[i]) 
	  {
	    len = rtf_strlen(arg_list->argv[i]);
		
		if (len)
		{
          mem_free(arg_list->argv[i],len);		
		}
	  }
    }

    return;
}

int main()
{
  int64 fsize;	
  unsigned_int console_handle;
  int i,argc;
  LPWSTR *argv;
  t_io_object_types io_object_type;
  t_rtf_options rtf_options;
  boolean destination_folder;
  	
  		
	
	
	console_handle = open_console(false);

	if (console_handle == INVALID_HANDLE_VALUE)
	{
		report_error(console_handle, GetLastError());
		return -1;
	}
		
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (!argv) 
	{
	  report_error(console_handle, GetLastError());
	  close_console(console_handle);
	  return -1;
	}
			
	if ((argc < 3) || (argc > 6))
	{
	  show_help(console_handle);
	  close_console(console_handle);
	  return ERROR_SUCCESS;
	}
		
	/*if (!convert_argument_list(args,&arg_list,arg_count))
	{
	  report_error(console_handle, GetLastError());
	  close_console(console_handle);
	  return -1;
	}

	LocalFree(args);*/
		
	destination_folder = false;

	rtf_memset(&rtf_options,0,sizeof(rtf_options));

	for (i = 1; i < argc; i++)
	{
	  
	  if (rtf_strlen(argv[i]) < 4) goto param_error;
	  
	  if (argv[i][0] == '-')
	  {
	    if ((rtf_options.destination_path.len) || (rtf_options.source_path.len)) goto param_error;
		
		switch (argv[i][1])
		{
		  case 'l':
		  case 'L': if (rtf_options.log_handle) goto param_error;
		            
					rtf_options.log_handle = create_file(L"log.txt");
					
					if (rtf_options.log_handle == INVALID_HANDLE_VALUE)
					{
					  report_error(console_handle,INVALID_HANDLE_VALUE);
					  return -1;
					}

	                break;				
					 
		  case 'o':
		  case 'O': if ((rtf_options.extract_ole) && (!rtf_options.extract_datastore)) goto param_error;
		            rtf_options.extract_ole = true;
			        break;
		  
		  case 'a':
		  case 'A': if (rtf_options.extract_datastore) goto param_error;
		            rtf_options.extract_datastore = rtf_options.extract_font = rtf_options.extract_datafield = rtf_options.extract_ole = rtf_options.extract_pictures = rtf_options.extract_overlays = rtf_options.extract_shapes = true;
			        break;

		  case 'e':
		  case 'E': if ((rtf_options.extract_overlays) && (!rtf_options.extract_datastore)) goto param_error;
		            rtf_options.extract_overlays = true;
			        break;
 
		  case 't':
		  case 'T': if (destination_folder++) goto param_error;
		            break;

		  default : goto param_error;			        
		}
	  }
	  else
	  {
	    if ((rtf_options.source_path.len) && (rtf_options.destination_path.len)) goto param_error;

		io_object_type = get_io_type(argv[i]);

		if (io_object_type == t_io_undefined_object) 
		{		  
		  report_error(console_handle,rtf_options.source_path.len == 0 ? SOURCE_FILE_ERROR : TARGET_FOLDER_ERROR);
		  return -1;
		}

		if (!rtf_options.source_path.len)
		{
		  rtf_options.source_path.len = rtf_strlen(argv[i]);
		  
		  if (((signed_int)rtf_options.source_path.len <= 0) || (rtf_options.source_path.len >= sizeof(rtf_options.source_path.data)))
		  {
		    report_error(console_handle,PATH_TOO_LONG_ERROR);
			return -1;
		  }
		  
		  rtf_memcpy(&rtf_options.source_path.data, argv[i], rtf_options.source_path.len);
		  
		  rtf_options.src_object_type = io_object_type;
		}
		else if (destination_folder)
		{
		  if (io_object_type != t_io_folder_object) 
		  {
		    report_error(console_handle,TARGET_FOLDER_ERROR);
			return -1;
		  }
		  
		  rtf_options.destination_path.len = rtf_strlen(argv[i]);
		  
		  if (((signed_int)rtf_options.destination_path.len <= 0) || (rtf_options.destination_path.len + 2 >= sizeof(rtf_options.destination_path.data)))
		  {
		    report_error(console_handle,PATH_TOO_LONG_ERROR);
			return -1;
		  }

		  rtf_memcpy(&rtf_options.destination_path.data,argv[i],rtf_options.destination_path.len);
		  
		  rtf_options.dest_object_type = io_object_type;
		  
		  break;
		}
	  }
	}
	
	//__asm _emit 0xEB
//	__asm _emit 0xFE

	if (!destination_folder)
	{
      puchar p;
	  t_io_path_infos path_infos;
	  
	  p = argv[0];

	  if (argv[0][1] != ':')
	  {
	    p = (puchar)&rtf_options.destination_path.data;
		rtf_options.destination_path.len = GetModuleFileName(GetModuleHandle(NULL),p,MAX_PATH * 2) * 2;
		if ((signed_int)rtf_options.destination_path.len <= 0) return -1;
	  }
	  else 
	  {
	    rtf_options.destination_path.len = rtf_strlen(p);

 	    if (((signed_int)rtf_options.destination_path.len <= 0) || (rtf_options.destination_path.len >= sizeof(rtf_options.destination_path.data)))
	    {
	      report_error(console_handle,PATH_TOO_LONG_ERROR);
		  return -1;
	    }
	    	  
	    rtf_memcpy(&rtf_options.destination_path.data,p,rtf_options.destination_path.len);
      }

	  if (!crack_io_path(&rtf_options.destination_path,&path_infos))
	  {
	    report_error(console_handle,GetLastError());
		return -1;
	  } 
	  
	  if (path_infos.file_name_len >= rtf_options.destination_path.len)
	  {
	   	report_error(console_handle,API_ERROR);
		return -1;
	  }	

	  rtf_options.destination_path.len = path_infos.path_len;
	  rtf_options.destination_path.data[path_infos.path_len] = 0;

	  rtf_options.dest_object_type = t_io_folder_object;
	}
		
	//free_argument_list(&arg_list);

	LocalFree(argv);

	rtf_options.console_handle = console_handle;
	
	write_console(console_handle, "\n", 1, t_undefined_console_text_mode);
	write_console(console_handle, RTF_DECOMPOSER_VERSION_STRING,rtf_strlenA(RTF_DECOMPOSER_VERSION_STRING), t_undefined_console_text_mode);
	write_console(console_handle, "\n\n\n", 3, t_undefined_console_text_mode);
	write_console(console_handle,"working..",9,t_undefined_console_text_mode);
	write_console(console_handle,"\n\n\n",3, t_undefined_console_text_mode);
	
	GetConsoleScreenBufferInfo(console_handle,&rtf_options.screen_coordinates);
	
	unpack_rtf(&rtf_options);
		
	if (rtf_options.log_handle)
	{
	  close_file(rtf_options.log_handle);
	}	  
	
	write_console(console_handle,"\n\n\n",3, t_undefined_console_text_mode);
	write_console(console_handle,"job done.\n", 10, t_undefined_console_text_mode);

	close_console(console_handle);
	
	return ERROR_SUCCESS;

	param_error :

	show_help(console_handle);

	report_error(console_handle, INVALID_PARAM_ERROR);
	
	close_console(console_handle);
	
	return -1;
}
