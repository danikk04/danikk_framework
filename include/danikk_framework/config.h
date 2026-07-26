#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/dictionary.h>

namespace danikk_framework
{
	template<class size_t> void inlineParseConfig(DynamicInlineStringDictionary<size_t>& out, char* data)
	{
		out.setData(data);

		size_t i = 0;
		size_t key_offset = 0;
		size_t value_offset = 0;
		size_t end = false;

		while(!end)
		{
			start:
			for(;data[i] == '\n'; i++);

			key_offset = i;

			while(true)//парсинг ключа
			{
				char chr = data[i];
				if(chr == '\0')
				{
					i++;
					goto start;
				}
				else if(chr == '=')
				{
					data[i] = '\0';
					i++;
					break;
				}
				else
				{
					i++;
				}
			}

			value_offset = i;

			while(true)//парсинг значения
			{
				char chr = data[i];

				if(chr == '\0')
				{
					end = true;
					break;
				}
				else if(chr == '\n')
				{
					data[i] = '\0';
					i++;
					break;
				}
				else
				{
					i++;
				}
			}

			out.addPair(key_offset, value_offset);
		}
	}
}
