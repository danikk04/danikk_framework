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
	template<class key_t, class value_t> class DynamicDictionary
	{
	public:
		struct pair
		{
			key_t key;
			value_t value;
		};
	private:
		DynamicArray<pair*> data;
	public:
		DynamicDictionary() = default;
		DynamicDictionary(const DynamicDictionary&) = delete;
		DynamicDictionary& operator=(const DynamicDictionary&) = delete;

		~DynamicDictionary()
		{
			for(pair* current : data)
			{
				current->~pair();
				::free(current);
			}
			data.clear();
		}

		pair& appendPair()
		{
			pair* value = structalloc<pair>();
			new (value) pair();
			data.push(value);
			return *value;
		}

		void insert(const key_t& key, const value_t& value)
		{
			if(value_t* existing = get(key))
			{
				*existing = value;
				return;
			}

			pair& new_pair = appendPair();
			new_pair.key = key;
			new_pair.value = value;
		}

		void insert(const key_t& key, value_t&& value)
		{
			if(value_t* existing = get(key))
			{
				*existing = danikk_framework::move(value);
				return;
			}

			pair& new_pair = appendPair();
			new_pair.key = key;
			new_pair.value = danikk_framework::move(value);
		}

		value_t* get(const key_t& key)
		{
			for(pair* current : data)
			{
				pair& p = *current;
				bool keys_equal;
				if constexpr(requires { p.key.data(); p.key.size(); key.data(); key.size(); })
				{
					keys_equal = p.key.size() == key.size()
						&& (p.key.size() == 0 || std::memcmp(p.key.data(), key.data(), p.key.size()) == 0);
				}
				else
				{
					keys_equal = p.key == key;
				}
				if(keys_equal)
				{
					return &p.value;
				}
			}
			return NULL;
		}
	};

	template<class len_t, size_t capacity> class InlineStringDictionary
	{
		char* m_data = NULL;
		struct key_value_pair
		{
			len_t key_offset;
			len_t value_offset;
		};
		key_value_pair m_array[capacity];
		size_t m_size = 0;
	public:
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

	template<class size_t> class DynamicInlineStringDictionary
	{
		char* m_data;
		struct key_value_pair
		{
			size_t key_offset;
			size_t value_offset;
		};
		DynamicArray<key_value_pair> m_array;
	public:
		void setData(char* data)
		{
			m_data = data;
		}

		void addPair(size_t key, size_t value)
		{
			m_array.push(key_value_pair{key, value});
		}

		void addPair(const char* key, const char* value)
		{
			m_array.push(key_value_pair{key - m_data, value - m_data});
		}

		template<class value_t> value_t get(const String& key)
		{
			return get<value_t>(key.c_string());
		}

		template<class value_t> value_t get(const char* key)
		{
			const char* result = NULL;
			for(const key_value_pair& pair : m_array)
			{
				const char* current_key = m_data + pair.key_offset;
				if (strcmp(current_key, key) == 0)
				{
					result = m_data + pair.value_offset;
					break;
				}
			}

			if constexpr(std::is_same<value_t, const char*>::value)
			{
				return result;
			}
			else if constexpr(is_number<value_t>::value)
			{
				if(result != NULL)
				{
					return parseNumber<value_t>(result);
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return value_t(result);
			}
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
