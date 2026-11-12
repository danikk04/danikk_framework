#pragma once

#include <iostream>
#include <type_traits>

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/generic_string.h>
#include "cstring_functions.h"

namespace danikk_framework
{
    class String : public GenericString
    {
    	friend class MemoryBuffer;
    private:
        char* m_data = nullptr;
        size_t m_size = 0;
        size_t m_capacity = 0;
        char m_empty = '\0';
    public:
        String();

        String(const String& other);

        String(const std::string& other);

        String(String&& other);

		String(const char* ptr);

		String(const char* begin, const char* end);

		String(const char* ptr, size_t size);

		String(char* ptr);

		String(char* ptr, size_t size);

		String(char* ptr, size_t size, size_t capacity);

		String(size_t size);

		String(size_t size, char initChar);

		String& operator=(const String& other)
		{
			if(this != &other)
			{
				reserve(other.m_size);
				if(other.m_size != 0)
				{
					memcpy(m_data, other.data(), other.m_size);
				}
				m_size = other.m_size;
				if(m_data != nullptr) m_data[m_size] = '\0';
				else m_empty = '\0';
			}
			return *this;
		}

		String& operator=(String&& other);

		~String();

	    operator std::string() const
		{
	    	return std::string(data(), m_size);
		}

	    operator bool() const;

		const char* data() const
		{
			return m_data != nullptr ? m_data : &m_empty;
		}

		const char* begin() const
		{
			return data();
		}

		const char* end() const
		{
			return data() + m_size;
		}

		char* data()
		{
			return m_data != nullptr ? m_data : &m_empty;
		}

		char* begin()
		{
			return data();
		}

		char* end()
		{
			return data() + m_size;
		}

		size_t size() const
		{
			return m_size;
		}

		size_t capacity() const
		{
			return m_capacity == 0 ? 0 : m_capacity - 1;
		}

		void resize(size_t new_size)
		{
			resize(new_size, '\0');
		}

		void resize(size_t new_size, char fill_character)
		{
			if(new_size > m_size)
			{
				reserve(new_size);
				memset(m_data + m_size, fill_character, new_size - m_size);
			}
			m_size = new_size;
			if(m_data != nullptr) m_data[m_size] = '\0';
			else m_empty = '\0';
		}

		void substractsize(size_t minus_size)
		{
			assert(minus_size <= m_size);
			m_size -= minus_size;
			if(m_data != nullptr) m_data[m_size] = '\0';
			else m_empty = '\0';
		}

		const char* c_string() const
		{
	        return m_data != nullptr ? m_data : "";
		}

		void reserve(size_t count);
    };
}
