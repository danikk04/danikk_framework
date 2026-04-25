#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/string.h>
#include <danikk_framework/type_traits.h>
#include <danikk_framework/number_base.h>
#include <danikk_framework/internal/generic_string.h>
#include <string>
#include <cstring>
#include <cstdlib>

namespace danikk_framework
{
	template<size_t _capacity> class StaticString : public GenericString
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

		const char* data() const
		{
			return m_data;
		}

		const char* begin() const
		{
			return m_data;
		}

		const char* end() const
		{
			return m_data + m_size;
		}

		char* data()
		{
			return m_data;
		}

		char* begin()
		{
			return m_data;
		}

		char* end()
		{
			return m_data + m_size;
		}

		size_t size() const
		{
			return m_size;
		}

		size_t capacity() const
		{
			return _capacity;
		}

		void resize(size_t new_size)
		{
			reserve(m_size + new_size);
			m_size = new_size;
		}

		void addsize(size_t add_size)
		{
			reserve(add_size);
			m_size += add_size;
		}

		void substractsize(size_t minus_size)
		{
			m_size -= minus_size;
		}

		void reserve(size_t add_size)
		{
			assert(m_size + add_size <= _capacity);
		}
	};
}
