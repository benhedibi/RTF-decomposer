#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "mallocator.h"


puchar mem_alloc(const unsigned_int mem_size)
{
  	
    if ((signed_int)mem_size <= 0)
	{
		SetLastError(API_ERROR);
		return NULL;
	}

	return (puchar)HeapAlloc(GetProcessHeap(),0,mem_size);	
}

void mem_free(const puchar mem, const unsigned_int mem_size)
{
	
	if ((mem) && ((signed_int)mem_size > 0))
	{
		HeapFree(GetProcessHeap(),0,mem);
	}

	return;


}