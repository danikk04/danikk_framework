#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/assert.h>

namespace danikk_framework
{
	template<class string_index_t> class MonolithCStringArray
	{
		//Сначала идёт блок с символьными индексами строк, потом уже сами символы
		struct
		{
			byte* m_data = NULL;
			size_t size;

			void allocate(size_t size)
			{
				this->size = size;
				m_data = malloc(size);
			}

			bool isNull()
			{
				return m_data == NULL;
			}

			byte* data()
			{
				return m_data;
			}
		} memory;

		struct
		{
			uint32 size = 0;
			uint32 capacity;

			string_index_t* data()
			{
				return (string_index_t*)memory.data();
			}

			string_index_t* end()
			{
				return data() + size;
			}

			void push()
			{
				assert(size < capacity);
				*end() = string_data.end();
				size++;
			}

			size_t byte_capacity()
			{
				return capacity * sizeof(string_index_t);
			}
		} index_data;

		struct
		{
			uint32 size = 0;
			uint32 capacity;

			char* data()
			{
				return (char*)memory.data() + index_data.byte_capacity();
			}

			char* end()
			{
				return data() + size;
			}

			void push(const char* value, size_t value_size)
			{
				assert(size + value_size < capacity);
				memcpy(end(), value, value_size);
				size += value_size;
				*end() = '\0';
				size++;
			}
		} string_data;
	public:
		void allocate(uint32 string_capacity, uint32 char_capacity)
		{
			assert(memory.isNull());
			string_data.capacity = string_capacity;
			index_data.capacity = char_capacity + string_data.capacity;//+ NULL терминаторы
			memory.allocate(string_data.capacity + index_data.byte_capacity());
		}

		MonolithCStringArray() = default;

		~MonolithCStringArray()
		{
			free((void*)memory.data());
		}

		void push(const String& data)
		{
			index_data.push();
			string_data.push(data.data(), data.size());
		}
	};

	typedef MonolithCStringArray<uint16> MonolithCStringArray16;
	typedef MonolithCStringArray<uint32> MonolithCStringArray32;
}
