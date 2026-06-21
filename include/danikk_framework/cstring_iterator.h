#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/cstring_functions.h>

namespace danikk_framework
{
	class CStringIterator
	{
	private:
		char* data;
	public:
		CStringIterator(char* data)
		{
			this->data = data;
		}

		//Пропускает текст, пока текущий символ совпадает с одним из заданных.
		void skipChars(const char* chars)
		{
			while(true)
			{
				char curchr = *data;
				if(strchr(chars, curchr))
				{
					return;
				}
				data++;
			}
		}

		void skipToChar(char chr)
		{
			data = strchr(data, chr);
			assert(data != NULL);
		}

		void skipToString(const char* str)
		{
			data = strstr(data, str);
			assert(data != NULL);
		}

		void operator++()
		{
			data++;
		}

		char& operator*()
		{
			return *data;
		}
	};
}
