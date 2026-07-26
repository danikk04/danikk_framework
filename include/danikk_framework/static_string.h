#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/string.h>
#include <danikk_framework/type_traits.h>
#include <danikk_framework/number_base.h>
#include <danikk_framework/internal/generic_string.h>
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
			return _capacity;
		}

		void resize(size_t new_size)
		{
			parent::resize(new_size);
		}

		void addsize(size_t plus_size)
		{
			parent::addsize(plus_size);
		}

		void substractsize(size_t minus_size)
		{
			parent::substractsize(minus_size);
		}

		void reserve(size_t new_size)
		{
			parent::reserve(new_size);
		}

		void clear()
		{
			parent::clear();
		}
	};
}
