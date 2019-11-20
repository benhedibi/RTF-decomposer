#pragma once

#include "common.h"

#define IS_ALIGNED_PTR(ptr) (((unsigned_int)(ptr) & (unsigned_int)(sizeof(unsigned_int) - 1)) == 0)


unsigned_int rtf_strlen(puchar str)
{
  unsigned_int len = 0;
  
   if (!str) return len;
    
   while (true)
   {
     if (*str == 0) break;
	 len += 2;
	 str += 2;   
   }

   return len;
}

unsigned_int rtf_strlenA(puchar str)
{
	unsigned_int len = 0;

	if (!str) return len;

	while (true)
	{
		if (*str++ == 0) break;
		len++;
		
	}

	return len;
}

signed_int rtf_itoh(const int value, const p_int_to_hexbuffer conversionbuffer)
{
   return 0;
}

puchar rtf_itoa(const int value, const p_int_to_strbuffer conversion_buffer)
{
	const uchar numbersIn10Radix[10] = { '0','1','2','3','4','5','6','7','8','9' };
	const uchar reverseNumbersIn10Radix[10] = { '9','8','7','6','5','4','3','2','1','0' };
	const uchar *reverseArrayEndPtr = &reverseNumbersIn10Radix[9];

	signed_int i = value;
	puchar p;


	if (!conversion_buffer) return 0;

	p = (puchar)conversion_buffer;

	p += sizeof(*conversion_buffer) - 1;

	*p = 0;

	if (i >= 0)
	{

		do
		{
			p -= 1;
			*p = numbersIn10Radix[i % 10];
			i /= 10;
		} while (i);

	}
	else
	{

		do
		{
			p -= 1;
			*p = reverseArrayEndPtr[i % 10];
			i /= 10;
		} while (i);

		p -= 1;
		*p = '-';
	}

	return p;
}

puchar rtf_witoa(const int value, const p_int_to_strbuffer conversion_buffer)
{
  const uchar numbersIn10Radix[10] = { '0','1','2','3','4','5','6','7','8','9' };
  const uchar reverseNumbersIn10Radix[10] = { '9','8','7','6','5','4','3','2','1','0' };
  const uchar *reverseArrayEndPtr = &reverseNumbersIn10Radix[9];
  
  signed_int i = value;
  puchar p;


	if (!conversion_buffer) return 0;

	p = (puchar)conversion_buffer;
	
	p += sizeof(*conversion_buffer) - 2;
	
	*(puint16)p = 0;

	if (i >= 0)
	{

		do
		{			
			*--p = 0;
			*--p = numbersIn10Radix[i % 10];
			i /= 10;
		} while ((i) && (p - (puchar)conversion_buffer >= 2));

	}
	else
	{

		do
		{
			*--p= 0;
			*--p = reverseArrayEndPtr[i % 10];
			i /= 10;
		} while ((i) && (p - (puchar)conversion_buffer > 2));

		*--p = 0;
		*--p = '-';
	}

	return p;
}

boolean rtf_memcmp(puchar src, puchar dest, unsigned_int len)
{

   if ((!src) || (!dest) || ((signed_int)len <= 0)) return false;

   len--;

   if (src[len] != dest[len]) return false;

   while (true)
   {
     if (*src++ != *dest++) return false;
	 len--;
	 if ((signed_int)len <= 0) break;
   }

   return true;
}

void rtf_memcpy(const puchar destination, const puchar source,unsigned_int len)
{
  punsigned_int pdw_dest, pdw_src;
  puchar p_dest, p_src;


	if ((!destination) || (!source) || ((signed_int)len <= 0)) return;
	
	p_dest = (puchar)destination;
	p_src = (puchar)source;

	while (!IS_ALIGNED_PTR(p_dest))
	{
	  *p_dest++ = *p_src++;
	  len--;
	  if (!len) return;
	}

	pdw_dest = (punsigned_int)p_dest;
	pdw_src = (punsigned_int)p_src;

	while (len >= sizeof(len))
	{
	  *pdw_dest++ = *pdw_src++;
	  len -= sizeof(len);
	}

	p_dest = (puchar)pdw_dest;
	p_src = (puchar)pdw_src;

	while (len)
	{
	  *p_dest++ = *p_src++;
	  len--;
	}

	return;
}

void rtf_memset(const puchar destination,const int value, const unsigned_int len)
{
  unsigned_int fill_value,size_left;
  puchar p; 
  punsigned_int pdw;
    
	if ((!destination) || ((signed_int)len <= 0)) return;
    
	fill_value = value;
	size_left = len;

    p = destination;

	while (!IS_ALIGNED_PTR(p))
	{
	  *(p++) = (byte)fill_value;
	  size_left--;
	  if (!size_left) return;
	}

	pdw = (punsigned_int)p;

	fill_value = (unsigned_int)fill_value << 24 | (unsigned_int)fill_value << 16 | (unsigned_int)fill_value << 8 | (unsigned_int)fill_value;

#ifdef _WIN64
	fill_value = fill_value << 32 | fill_value;
#endif

	while (size_left >= sizeof(size_left)) // no loop unrolling, juste laisser le module heuristique du compilateur faire son travail ;)
	{
	  *pdw++ = fill_value;
	  size_left -= sizeof(size_left);
	}

	p = (puchar)pdw;

	while (size_left)
	{
	  *p++ = (uint8)fill_value;
	  size_left--;
	}

	return;
}
