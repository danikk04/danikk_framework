#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/string.h>
#include <danikk_framework/type_traits.h>
#include <danikk_framework/number_base.h>

#include <string>
#include <cstring>
#include <cstdlib>

namespace danikk_framework
{
	template<size_t _capacity> class StaticString
	{
	private:
		char m_data[_capacity];
		size_t m_size = 0;

		void write(const char* data, size_t size)
		{
			memcpy(end(), data, size);
			m_size += size;
		}
	public:
		StaticString()
		{
		}

		StaticString(StaticString& other)
		{
			memcpy(m_data, other.m_data, other.size());
			m_size = other.m_size;
		}

		StaticString(StaticString&& other)
		{
			memcpy(m_data, other.m_data, other.size());
			m_size = other.m_size;
		}

		StaticString& operator=(StaticString& other)
		{
			memcpy(m_data, other.m_data, other.size());
			m_size = other.m_size;
		}

		template<class str_t> StaticString(const str_t& data)
		{
			if constexpr(is_cstring<str_t>::value)
			{
				const char* ptr = data;
				char* push_ptr = m_data;
				while(*ptr != '\0')
				{
					*push_ptr = *ptr;
					push_ptr++;
					ptr++;
					m_size++;
				}
			}
			else
			{
				size_t data_size = data.size();
				if(capacity() < data_size)
				{
					throw "[danikk_framework::StringBuffer] capacity < data_size";
				}
				memcpy(m_data, data.data(), data_size);
				m_size = data_size;
			}
		}

		template<class str_t> StaticString& operator =(const str_t& data)
		{
			new (this) StaticString<_capacity>(data);
			return *this;
		}

		bool sizeWillFit(size_t checkSize)
		{
			return checkSize < (capacity - size());
		}

		bool willFit(const char* data)
		{
			return sizeWillFit(strlen(data));
		}

		bool willFit(const String& data)
		{
			return sizeWillFit(data.size());
		}

		StaticString& operator << (char data)
		{
			*end() = data;
			m_size++;
			return *this;
		}

		StaticString& operator << (const char* data)
		{
			int len = strlen(data);
			memcpy(end(), data, len);
			m_size += len;
			return *this;
		}

		StaticString& operator << (unsigned long int num)
		{
			char buffer[64];
			snprintf(buffer, 64, "%lu", num);
			*this << (const char*)buffer;
			return *this;
		}

		StaticString& operator << (const String& str)
		{
			memcpy((void*)end(), (const void*)str.data(), str.size());
			m_size += str.size();
			return *this;
		}

		void resize(size_t n_size)
		{
			m_size = n_size;
		}

		const char* c_string()
		{
			*end() = '\0';
			return m_data;
		}

		const char* c_string() const
		{
			*end() = '\0';
			return m_data;
		}

		char* data()
		{
			return m_data;
		}

		const char* data() const
		{
			return m_data;
		}

		char* end()
		{
			return data() + m_size;
		}

		const char* end() const
		{
			return data() + m_size;
		}

		void clear()
		{
			m_size = 0;
		}

		char& operator [](size_t index)
		{
			return m_data[index];
		}

		size_t capacity() const
		{
			return _capacity;
		}

		size_t size() const
		{
			return (size_t)m_size;
		}

		char firstChar()
		{
			return *m_data;
		}

		char lastChar()
		{
			return *(end() - 1);
		}
	};

	template<class stream_t, size_t string_buffer_size> stream_t& operator <<(stream_t& stream,const StaticString<string_buffer_size>& data)
	{
		const char* current = data.data();
		const char* endptr = data.end();

		while(current < endptr)
		{
			stream << *current;
			current++;
		}
		return stream;
	}
}
