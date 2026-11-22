#pragma once

#include <new>
#include <utility>

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/assert.h>
#include <danikk_framework/memory.h>

namespace danikk_framework
{
	template <class value_t, size_t m_capacity, class size_t = size_t> class Array
	{
	private:
		value_t m_data[m_capacity];
		size_t m_size = 0;

		void assertPushSize() const
		{
			assert(m_size < m_capacity);
		}

		void resetElement(size_t index)
		{
			m_data[index].~value_t();
			new (&m_data[index]) value_t();
		}

		template<class... args_t>
		void constructElement(size_t index, args_t&&... args)
		{
			m_data[index].~value_t();
			try
			{
				new (&m_data[index]) value_t(std::forward<args_t>(args)...);
			}
			catch(...)
			{
				new (&m_data[index]) value_t();
				throw;
			}
		}

	public:
		typedef value_t value_type;
		typedef Array<value_t, m_capacity> this_type;

		Array() = default;

		Array(const value_t& value, size_t count = 1) : Array()
		{
			assert(count <= m_capacity);
			while(m_size < count)
			{
				constructElement(m_size, value);
				++m_size;
			}
		}

		Array(const InitList<value_t>& data) : Array()
		{
			assert(data.size() <= m_capacity);
			while(m_size < data.size())
			{
				constructElement(m_size, data.begin()[m_size]);
				++m_size;
			}
		}

		Array(const this_type& other) : Array()
		{
			while(m_size < other.m_size)
			{
				constructElement(m_size, other.m_data[m_size]);
				++m_size;
			}
		}

		Array(this_type&& other) : Array()
		{
			while(m_size < other.m_size)
			{
				constructElement(m_size, std::move(other.m_data[m_size]));
				++m_size;
			}
			other.clear();
		}

		~Array() { clear(); }

		this_type& operator =(const this_type& other)
		{
			if(this == &other) return *this;
			clear();
			while(m_size < other.m_size)
			{
				constructElement(m_size, other.m_data[m_size]);
				++m_size;
			}
			return *this;
		}

		this_type& operator =(this_type&& other)
		{
			if(this == &other) return *this;
			clear();
			while(m_size < other.m_size)
			{
				constructElement(m_size, std::move(other.m_data[m_size]));
				++m_size;
			}
			other.clear();
			return *this;
		}

		bool operator==(const Array<value_t, m_capacity>& other) const
		{
			if(m_size != other.m_size) return false;
			for(size_t index = 0; index < m_size; ++index)
				if(!(m_data[index] == other.m_data[index])) return false;
			return true;
		}

		value_t& push(const value_t& element)
		{
			assertPushSize();
			value_t& result = m_data[m_size];
			constructElement(m_size, element);
			++m_size;
			return result;
		}

		void push_move(value_t&& element)
		{
			assertPushSize();
			constructElement(m_size, std::move(element));
			++m_size;
		}

		inline void resize(size_t new_size)
		{
			assert(new_size <= m_capacity);
			while(m_size > new_size) resetElement(--m_size);
			while(m_size < new_size) ++m_size;
		}

		void addsize(size_t add_size)
		{
			reserve(add_size);
			m_size += add_size;
		}

		void substractsize(size_t minus_size)
		{
			assert(minus_size <= m_size);
			resize(m_size - minus_size);
		}

		void reserve(size_t add_size)
		{
			assert(m_size + add_size <= m_capacity);
		}

		value_t& pushCtor()
		{
			assertPushSize();
			constructElement(m_size);
			value_t& result = m_data[m_size];
			++m_size;
			return result;
		}

		template<class... args_t> value_t& pushCtor(args_t&&... args)
		{
			assertPushSize();
			constructElement(m_size, std::forward<args_t>(args)...);
			value_t& result = m_data[m_size];
			++m_size;
			return result;
		}

		inline value_t pop()
		{
			assert(m_size > 0);
			value_t result = std::move(m_data[--m_size]);
			resetElement(m_size);
			return result;
		}

		inline value_t& peek()
		{
			assert(m_size > 0);
			return m_data[m_size - 1];
		}

		void replace(const value_t& from, const value_t& to)
		{
			for(value_t& value : *this)
				if(value == from) value = to;
		}

		value_t pull(index_t index)
		{
			assert(index < m_size);
			if(index == size() - 1) return pop();
			value_t result = std::move(m_data[index]);
			constructElement(index, std::move(m_data[m_size - 1]));
			resetElement(--m_size);
			return result;
		}

		bool isEmpty() const { return m_size == 0; }

		value_t* data() { return m_data; }
		const value_t* data() const { return m_data; }
		size_t size() const { return m_size; }
		size_t capacity() const { return m_capacity; }

		index_t indexOf(const value_t& target) const
		{
			for(index_t index = 0; index < size(); ++index)
				if(m_data[index] == target) return index;
			return ERROR_INDEX;
		}

		inline value_t& last()
		{
			assert(m_size > 0);
			return m_data[m_size - 1];
		}

		inline const value_t& last() const
		{
			assert(m_size > 0);
			return m_data[m_size - 1];
		}

		inline value_t& operator[](size_t index)
		{
			assert(index < m_size);
			return m_data[index];
		}

		inline const value_t& operator[](size_t index) const
		{
			assert(index < m_size);
			return m_data[index];
		}

		inline value_t* begin() { return m_data; }
		inline value_t* end() { return m_data + m_size; }
		inline const value_t* begin() const { return m_data; }
		inline const value_t* end() const { return m_data + m_size; }

		inline void clear()
		{
			while(m_size > 0) resetElement(--m_size);
		}

		void select(auto where, Array<value_t, m_capacity>& result) const
		{
			select<m_capacity>(where, result);
		}

		template<size_t other_array_capacity> void select(auto where,
			Array<value_t, other_array_capacity>& result) const
		{
			for(const value_t& value : *this)
				if(where(value)) result.push(value);
		}

		template<class array_t> void indexesOf(const value_t& element, array_t& result)
		{
			for(size_t index = 0; index < size(); ++index)
				if(m_data[index] == element) result.push(index);
		}

		value_t& minmax(bool is_max)
		{
			assert(m_size > 0);
			value_t* result = &m_data[0];
			for(value_t& value : *this)
				if(is_max ? value > *result : value < *result) result = &value;
			return *result;
		}

		value_t& duplicateLast()
		{
			push(last());
			return last();
		}
	};
}
