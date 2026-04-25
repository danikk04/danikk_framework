#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/dynamic_array.h>
#include <danikk_framework/string.h>

namespace danikk_framework
{
	template<class len_t, size_t capacity> class StaticInlineStringDictionary
	{
		const char* m_data;
		struct key_value_pair
		{
			len_t key_offset;
			len_t value_offset;
		};
		key_value_pair m_array[capacity];
		size_t m_size;
	public:

		StaticInlineStringDictionary()
		{
			m_data = NULL;
			m_size = 0;
		}

		void setData(char* data)
		{
			m_data = data;
		}

		void addPair(len_t key, len_t value)
		{
			m_array[m_size++] = key_value_pair{key, value};
		}
	};
}
