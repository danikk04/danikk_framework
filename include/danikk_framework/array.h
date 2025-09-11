#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/assert.h>
#include <danikk_framework/memory.h>

namespace danikk_framework
{
	template <class value_t, size_t m_capacity> class Array
	{
	private:
		value_t m_data[m_capacity];

		size_t m_size = 0;

		void assertPushSize()
		{
			assert(m_size < m_capacity);
		}
	public:
		typedef value_t value_type;
		typedef Array<value_t, m_capacity> this_type;

		Array(const value_t& value, size_t count = 1)
		{
			m_size = count;
			value_t* cur = (value_t*)m_data;

			while(count --> 0)
			{
				*cur = value;
				cur++;
			}
		}

		constexpr Array()
		{

		}

		Array(const this_type& other)
		{
			memcpy(this, &other, sizeof(this_type));
		}

		Array(const InitList<value_t>& data) : Array()
		{
			assert(data.size() <= m_capacity);
			const value_t* data_ptr = data.begin();
			for(index_t i = 0; i < data.size(); i++)
			{
				m_data[i] = data_ptr[i];
			}
			this->m_size = data.size();
		}

		void operator =(const this_type& other)
		{
			memcpy(this, &other, sizeof(this_type));
		}

		void operator =(this_type&& other)
		{
			memcpy(this, &other, sizeof(this_type));
		}

		bool operator==(const Array<value_t, m_capacity>& other) const
		{
			return this->m_size == other.m_size && memcmp((const char*)m_data, (const char*)other.m_data, m_size * sizeof(value_t));
		}

		value_t& push(const value_t& element)
		{
			assertPushSize();
			value_t& res = m_data[m_size++];
			res = element;
			return res;
		}

		void push_move(value_t&& element)
		{
			assertPushSize();
			new (&m_data[m_size++]) value_t(move(element));
		}

		inline void resize(size_t _size)
		{
			assert(m_size <= m_capacity);
			m_size = _size;
		}

		value_t& pushCtor()
		{
			assertPushSize();
			value_t& result = m_data[m_size];
			m_size++;
			new (&result) value_t();
			return result;
		}

		template<class... args_t> value_t& pushCtor(args_t&... args)
		{
			assertPushSize();
			value_t& result = m_data[m_size];
			m_size++;
			new (&result) value_t(args...);
			return result;
		}

		inline value_t pop()
		{
			return move(m_data[--m_size]);
		}

		inline value_t& peek()
		{
			return m_data[m_size - 1];
		}

		void replace(const value_t& from, const value_t& to)
		{
			for(value_t& v : *this)
			{
				if(v == from)
				{
					v = to;
				}
			}
		}

		value_t pull(index_t index)
		{
			if(index == size() - 1)
			{
				return pop();
			}
			else
			{
				value_t result = move(m_data[index]);
				m_data[index] = move(pop());
				return result;
			}
		}

		inline bool isEmpty()
		{
			return m_size == 0;
		}

		inline value_t* data()
		{
			return (value_t*)m_data;
		}

		inline const value_t* data() const
		{
			return (value_t*)m_data;
		}

		inline size_t size() const
		{
			return m_size;
		}

		inline size_t capacity() const
		{
			return m_capacity;
		}

		index_t indexOf(const value_t& target)
		{
			for(index_t i = 0; i < size(); i++)
			{
				if(m_data[i] == target)
				{
					return i;
				}
			}
			return ERROR_INDEX;
		}

		inline value_t& last()
		{
			return m_data[m_size - 1];
		}

		inline value_t& operator[](size_t index)
		{
			return m_data[index];
		}

		inline value_t* begin()
		{
			return m_data;
		}

		inline value_t* end()
		{
			return m_data + m_size;
		}

		inline const value_t* begin() const
		{
			return m_data;
		}

		inline const value_t* end() const
		{
			return m_data + m_size;
		}

		inline void clear()
		{
			m_size = 0;
		}

		void select(auto where, Array<value_t, m_capacity>& result) const
		{
			select<m_capacity>(where, result);
		}

		template<size_t other_array_capacity> void select(auto where,
				Array<value_t,other_array_capacity>& result) const
		{
			for(const value_t& value : *this)
			{
				if(where(value))
				{
					result.push(value);
				}
			}
		}

		template<class array_t> void indexesOf(const value_t& element, array_t& result)
		{
			for(size_t i = 0; i < size(); i++)
			{
				if(m_data[i] == element)
				{
					result.push(i);
				}
			}
		}

		value_t& minmax(bool is_max)
		{
			value_t* result = &m_data[0];
			for(value_t& value : *this)
			{
				if(is_max ? value > *result : value < *result)
				{
					result = &value;
				}
			}
			return *result;
		}

		value_t& duplicateLast()
		{
			push(last());
			return last();
		}
	};
}
