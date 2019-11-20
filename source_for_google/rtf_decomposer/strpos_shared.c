#include "strpos_shared.h"

#ifdef _WIN64
#define STR_CASE_MASK  0x2020202020202020
#define WSTR_CASE_MASK 0x0020002000200020
#else
#define STR_CASE_MASK  0x20202020
#define WSTR_CASE_MASK 0x00200020
#endif

boolean cmpAString(const puchar target_str1, const puchar target_str2,unsigned_int len)
{
  punsigned_int pdw_str1, pdw_str2;
  puchar str1, str2;

    pdw_str1 = target_str1;
	pdw_str2 = target_str2;

	if ((!pdw_str1) || (!pdw_str2) || ((signed_int)len <= 0)) return false;
	
	while (len >= sizeof(len))
    {
	  if ((*(pdw_str1) | STR_CASE_MASK) != ((*pdw_str2) | STR_CASE_MASK)) return false;
	  
	  pdw_str1++;
	  pdw_str2++;
      
	  len -= sizeof(len);
	}

	str1 = (puchar)pdw_str1;
	str2 = (puchar)pdw_str2;

	while (len)
    {
	  if ((*(str1) | 0x20) != ((*str2) | 0x20)) return false;
	  str1++;
	  str2++;
	  len--;
	}

	return true;
}

boolean cmpWString(const puchar target_str1, const puchar target_str2,unsigned_int len)
{
  punsigned_int pdw_str1, pdw_str2;
  puchar str1, str2;

	pdw_str1 = target_str1;
	pdw_str2 = target_str2;

	if ((!pdw_str1) || (!pdw_str2) || ((signed_int)len < 2)) return false;

	while (len >= sizeof(len))
	{
	  if ((*(pdw_str1) | WSTR_CASE_MASK) != ((*pdw_str2) | WSTR_CASE_MASK)) return false;

	  pdw_str1++;
	  pdw_str2++;

	  len -= sizeof(len);
	}

	str1 = (puchar)pdw_str1;
	str2 = (puchar)pdw_str2;

	while (len)
	{
		if ((*(str1) | 0x20) != ((*str2) | 0x20)) return false;
		str1 += 2;
		str2 += 2;
		len -= 2;
	}

	return true;
}

boolean cmpWAString(const puchar unicode_str, const puchar ansi_str,unsigned_int len)
{
  puchar pwstr, pstr;


    if ((!unicode_str) || (!ansi_str) || ((signed_int)len < 2)) return false;
	
	pwstr = unicode_str;
	pstr = ansi_str;

	while (true)
	{
      if ((*pwstr | 0x20) != (*pstr | 0x20)) return false;

	  len -= 2;

	  if ((signed_int)len < 0) break;

	  pwstr += 2;
	  
	  pstr++;		
	}

	return true;
}

boolean cmpAWString(const puchar ansi_str, const puchar unicode_str, unsigned_int len)
{
  puchar pwstr, pstr;


    if ((!unicode_str) || (!ansi_str) || ((signed_int)len < 2)) return false;
	
	pwstr = unicode_str;
	pstr = ansi_str;

	while (true)
	{
	  if ((*pwstr | 0x20) != (*pstr | 0x20)) return false;

	  len--;

	  if ((signed_int)len < 0) break;

	  pwstr += 2;

	  pstr++;	  
	}

	return true;
}
