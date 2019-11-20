#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "io.h"
#include "common.h"


unsigned_int open_file(const puchar file_path)
{	
	return (unsigned_int)CreateFile(file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);		
}

unsigned_int create_file(const puchar file_path)
{
	return (unsigned_int)CreateFile(file_path, (GENERIC_READ | GENERIC_WRITE), (FILE_SHARE_READ | FILE_SHARE_WRITE), NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
}

void close_file(const unsigned_int file_handle)
{
	
	if ((signed_int)file_handle > 0)
	{
	  CloseHandle(file_handle);
	}

	return;
}

int64 get_file_size(const unsigned_int file_handle)
{
  LARGE_INTEGER fsize;

   
    if ((signed_int)file_handle <= 0)
    {
		SetLastError(API_ERROR);
		return IO_ERROR;
	}
	else if (!GetFileSizeEx(file_handle, &fsize)) return IO_ERROR;
	else return (int64)fsize.QuadPart;

}

boolean read_file(const unsigned_int file_handle, const puchar read_buffer, const unsigned_int read_size)
{
  unsigned_int rbytes;

	
    if (((signed_int)file_handle <= 0) || (!read_buffer) || ((signed_int)read_size <= 0))
	{
		SetLastError(API_ERROR);
		return false;
	}

	return (boolean)ReadFile(file_handle, read_buffer, read_size, &rbytes, NULL);
}

boolean write_file(const unsigned_int file_handle, const puchar write_buffer, const unsigned_int write_size)
{
  unsigned_int rbytes;

	
    if (((signed_int)file_handle <= 0) || (!write_buffer) || ((signed_int)write_size <= 0))
	{
		SetLastError(API_ERROR);
		return false;
	}

	return (boolean)WriteFile(file_handle, write_buffer, write_size, &rbytes, NULL);
}

t_io_object_types get_io_type(const puchar path)
{
  unsigned_int handle;
  WIN32_FIND_DATA fd;
    

	if (!path) return t_io_undefined_object;

	handle = FindFirstFile(path, &fd);

	if (handle == INVALID_HANDLE_VALUE) return t_io_undefined_object;
    
	FindClose(handle);
	
	if (TEST_ATTRIBUTE(fd.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY)) return t_io_folder_object;
	else return t_io_file_object;
	
}

unsigned_int read_dir(const p_string dir_path, const p_read_dir_params params)
{
  WIN32_FIND_DATA fd;
  unsigned_int handle,result,dir_len,len;

    
    result = 0;
	
    if ((IS_INVALID_STRING(dir_path)) || (!params) || (!params->read_callback) || (!params->user_data))
    {
		SetLastError(API_ERROR);
		return result;
	}

	dir_len = dir_path->len + 2;

	if ((++params->rec_level > 48) || (dir_len >= sizeof(dir_path->data)))
	{
	  SetLastError(PATH_TOO_LONG_ERROR);
	  return result;
	}

	if (dir_path->data[dir_path->len - 2] != '\\')
	{
	  dir_path->data[dir_path->len++] = '\\';
      dir_path->data[dir_path->len++] = 0;
	}  	
	
	dir_path->data[dir_path->len++] = '*';
	dir_path->data[dir_path->len++] = 0;
	dir_path->data[dir_path->len] = 0;

	handle = FindFirstFile(dir_path->data,&fd);

	if (handle == INVALID_HANDLE_VALUE) return result;
	
	while (true)
	{

	  while (fd.cFileName[0] == '.')
	  {
	    if (!FindNextFile(handle,&fd)) goto fin;
	  }
	       
	  len = rtf_strlen(&fd.cFileName);

	  if (dir_len + len >= sizeof(dir_path->data))
	  {
	    SetLastError(PATH_TOO_LONG_ERROR);
	    break;
	  }

	  result++;

	  dir_path->len = len + dir_len;
	  
	  dir_path->data[dir_path->len] = 0;
	  
	  rtf_memcpy(&dir_path->data[dir_len],&fd.cFileName,len);

	  if (TEST_ATTRIBUTE(fd.dwFileAttributes,FILE_ATTRIBUTE_DIRECTORY))
	  {
	    result += read_dir(dir_path,params);
	  }
	  else 
	  {
	    if (!params->read_callback(params->user_data,dir_path)) break;
      }

	  if (!FindNextFile(handle,&fd)) break;
	}  
	
	fin :

	params->rec_level--;

	if ((signed_int)params->rec_level < 0) 
	{
	  params->rec_level = 0;
	}

	FindClose(handle);

	return result;
}

boolean crack_io_path(const p_string path, const p_io_path_infos io_path_infos)
{
    
	io_path_infos->path = &path->data;
	io_path_infos->path_len = path->len;

	io_path_infos->file_name = io_path_infos->path + (io_path_infos->path_len - 2);
	io_path_infos->file_name_len = 0;

	while (true)
	{
	  if (*io_path_infos->file_name == '\\') break;
	  
	  io_path_infos->file_name_len += 2;

	  io_path_infos->file_name -= 2;

	  io_path_infos->path_len -= 2;

	  if (io_path_infos->path_len < 6) return false;
	}

	io_path_infos->file_name += 2;
	
	//io_path_infos->file_name_len--;
	
	return true;
}

unsigned_int open_console(const boolean input_mode)
{
	return (unsigned_int)GetStdHandle((input_mode == true) ? STD_INPUT_HANDLE : STD_OUTPUT_HANDLE);
}

void close_console(const unsigned_int console_handle)
{
	
	if ((signed_int)console_handle > 0)
	{
		CloseHandle(console_handle);
	}

	return;
}

boolean write_console(const unsigned_int console_handle, const puchar msg, const unsigned_int msg_size, const t_console_text_modes text_mode)
{
  uint16 color;
  boolean result;


    if (((signed_int)console_handle <= 0) || (!msg) || ((signed_int)msg <= 0))
	{
		SetLastError(API_ERROR);
		return false;
	}

	switch (text_mode)
	{
	  case t_undefined_console_text_mode : color = CONSOLE_DEFAULT_COLOR;
		                                   break;

	      case t_infos_console_text_mode : color = (FOREGROUND_GREEN/* | FOREGROUND_INTENSITY*/);
			                               break;

		case t_warning_console_text_mode : color = 14;//(FOREGROUND_GREEN | FOREGROUND_BLUE);
			                               break;

		  case t_error_console_text_mode : color = (FOREGROUND_RED | FOREGROUND_INTENSITY);
			                               break;
		  
		                         default : color = CONSOLE_DEFAULT_COLOR;
									       break;
	}

	SetConsoleTextAttribute(console_handle, color);

	result = write_file(console_handle, msg, msg_size);

	SetConsoleTextAttribute(console_handle, CONSOLE_DEFAULT_COLOR);
	
	return result;
}

boolean inline_console(const unsigned_int console_handle, const puchar line, const unsigned_int line_len, COORD *coordinates, const t_console_text_modes text_mode)
{
  unsigned_int numberOfCharsWritten = 0;
  uint16 color;
  boolean result;


	if (((signed_int)console_handle <= 0) || (!line) || ((signed_int)line_len <= 0) || (!coordinates))
	{
		SetLastError(API_ERROR);
		return false;
	}

	switch (text_mode)
	{
	case t_undefined_console_text_mode: color = CONSOLE_DEFAULT_COLOR;
		break;

	case t_infos_console_text_mode: color = (FOREGROUND_GREEN/* | FOREGROUND_INTENSITY*/);
		break;

	case t_warning_console_text_mode: color = 14;//(FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;

	case t_error_console_text_mode: color = (FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;

	default: color = CONSOLE_DEFAULT_COLOR;
		break;
	}

	SetConsoleTextAttribute(console_handle, color);

	result = WriteConsoleOutputCharacterA(console_handle,line,line_len,*coordinates,&numberOfCharsWritten);

	SetConsoleTextAttribute(console_handle, CONSOLE_DEFAULT_COLOR);

	return result;

}
