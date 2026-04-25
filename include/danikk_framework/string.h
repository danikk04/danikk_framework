#pragma once

#include <iostream>
#include <type_traits>

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/internal/generic_string.h>
#include "cstring_functions.h"

namespace danikk_framework
{
    class String : public GenericString
    {
    	friend class MemoryBuffer;
    private:
        char* m_data;//В c_string нужно ставить нуль терминатор в конец строки.
        size_t m_size;
        size_t m_capacity;

        void pushMemory(const char* memory, size_t size);
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

		String& operator=(const String&) = default;

		String& operator=(String&& other)
		{
			void* other_mem = (void*)&(String&)other;
			memcpy((void*)this, other_mem, sizeof(String));
			memset(other_mem, 0, sizeof(String));
			return *this;
		}

		~String();

	    operator const std::string&() const
		{
	    	return *((const std::string*)this);
		}

	    operator bool() const;

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
			return m_capacity;
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

		const char* c_string() const
		{
			const_cast<String*>(this)->reserve(1);
	        m_data[size()] = '\0';
	        return data();
		}

		void reserve(size_t count);
    };
}
