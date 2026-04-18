#pragma once

#include <danikk_framework/string.h>
#include <danikk_framework/cstring_functions.h>
#include "static_string.h"

namespace danikk_framework
{
	inline static const char* hex_source = "0123456789ABCDEF";

	//n- это размер input, а размер output это n * 2;
	inline void tohex(char* output, const byte* input, size_t n)
	{
		const byte* current = input + n - 1;

	    while(current >= input)
	    {
	    	uint8 chr = *current;
	        current--;
	        *output = hex_source[(chr >> 4) & 0x0F];
	        output++;
	        *output = hex_source[chr 		& 0x0F];
	        output++;
	    }
	}

	template<class num> inline void numtohex(char* output, num number)
	{
		tohex(output, (const byte*)&number, sizeof(num));
	}

	inline void ptrtohex(char* output, void* ptr)
	{
		numtohex(output, (size_t)ptr);
	}
}
