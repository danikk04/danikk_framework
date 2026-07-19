#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/dynamic_array.h>
#include <danikk_framework/string.h>
#include <danikk_framework/number.h>
#include <danikk_framework/format.h>
#include <danikk_framework/cstring_functions.h>
#include "array.h"

namespace danikk_framework
{
	template<class valueT> class StringKeyDictionary
	{
	public:
		struct pair
		{
			String key;
			valueT value;
		};
	private:
		DynamicArray<pair> data;
	public:
		StringKeyDictionary() = default;

		template<class key_t> void insert(const key_t& key, const valueT& value)
		{
			pair& new_pair = data.pushCtor();
			new_pair.key = key;
			new_pair.value = value;
		}

		template<class key_t> void insert(const key_t& key, valueT& value)
		{
			pair& new_pair = data.pushCtor();
			new_pair.key = key;
			new_pair.value = value;
		}

		valueT* get(const char* key)
		{
			for(pair& p : data)
			{
				if(strequal(p.key.c_string(), key))
				{
					return &p.value;
				}
			}
			return NULL;
		}

		valueT* get(const String& key)
		{
			for(pair& p : data)
			{
				if(p.key == key)
				{
					return &p.value;
				}
			}
			return NULL;
		}
	};

	template<class len_t, size_t capacity> class StaticInlineStringDictionary
	{
		char* m_data;
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

		void addPair(const char* key, const char* value)
		{
			m_array[m_size++] = key_value_pair{key - m_data, value - m_data};
		}
	};

	template<class len_t> class InlineStringDictionary
	{
		char* m_data;
		struct key_value_pair
		{
			len_t key_offset;
			len_t value_offset;
		};
		DynamicArray<key_value_pair> m_array;
	public:
		void setData(char* data)
		{
			m_data = data;
		}

		void addPair(len_t key, len_t value)
		{
			m_array.push(key_value_pair{key, value});
		}

		void addPair(const char* key, const char* value)
		{
			m_array.push(key_value_pair{key - m_data, value - m_data});
		}
	};

	/*template<size_t keys_capacity, size_t data_capacity> class StaticStringStackDictionary
	{
		Array<char*, keys_capacity> keys;
		Array<char, data_capacity> data;
	public:
		void insert(const char* key, const char* value)
		{
			char* data_ptr = data.end();
			size_t data_remains = data_capacity - data.size();
			size_t wrote_size = strlcpy(data_ptr, key, data_remains);
			assert(wrote_size <= data_remains);
		}
	};*/
}
