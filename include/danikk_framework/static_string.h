#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/string.h>
#include <danikk_framework/type_traits.h>
#include <danikk_framework/number_base.h>
#include <danikk_framework/generic_string.h>
#include <danikk_framework/assert.h>
#include <danikk_framework/array.h>
#include <string>
#include <cstring>
#include <cstdlib>

namespace danikk_framework
{
	template<size_t _capacity> class StaticString : public GenericString, Array<char, _capacity>
	{
	public:
		StaticString()
		{
			data()[0] = '\0';
		}

		typedef Array<char, _capacity> parent;

		const char& operator[](size_t index)
		{
			return data()[index];
		}

		const char* data() const
		{
			return parent::data();
		}

		const char* begin() const
		{
			return parent::begin();
		}

		const char* end() const
		{
			return parent::end();
		}

		char* data()
		{
			return parent::data();
		}

		char* begin()
		{
			return parent::begin();
		}

		char* end()
		{
			return parent::end();
		}

		size_t size() const
		{
			return parent::size();
		}

		size_t capacity() const
		{
			return _capacity == 0 ? 0 : _capacity - 1;
		}

		void resize(size_t new_size)
		{
			assert(new_size <= capacity());
			if(new_size > size())
			{
				memset(data() + size(), 0, new_size - size());
			}
			parent::resize(new_size);
			data()[new_size] = '\0';
		}

		void resize(size_t new_size, char fill_character)
		{
			assert(new_size <= capacity());
			if(new_size > size())
			{
				memset(data() + size(), fill_character, new_size - size());
			}
			parent::resize(new_size);
			data()[new_size] = '\0';
		}

		void substractsize(size_t minus_size)
		{
			assert(minus_size <= size());
			parent::substractsize(minus_size);
			data()[size()] = '\0';
		}

		void reserve(size_t new_size)
		{
			assert(new_size <= capacity());
		}

		void clear()
		{
			resize(0);
		}
	};
}
