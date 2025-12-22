#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/assert.h>
#include <cstdlib>
#include <danikk_framework/memory.h>

namespace danikk_framework
{
	template <class value_t> class DynamicArray
	{
	private:
		value_t* m_data = NULL;
		size_t m_size;
		size_t m_capacity;

		static constexpr size_t elementSize = sizeof(value_t);
	public:
		typedef value_t* iterator;
		typedef DynamicArray<value_t> type;
		typedef value_t value_type;

		DynamicArray()
		{
			m_data = 0;
			m_size = 0;
			m_capacity = 0;
		}
		DynamicArray(const DynamicArray<value_t>& other)
		{
			m_size = other.size();
			m_capacity = m_size;
            m_data = (value_t*)malloc(size());
            const value_t* read_ptr = other.begin();
            value_t* write_ptr = this->begin();
            const value_t* end_read_ptr = other.end();
            while(read_ptr < end_read_ptr)
            {
            	const value_t& read = *read_ptr++;
            	value_t& write = *write_ptr++;
            	write = read;
            }
            //memcpy(m_data, other.m_data, m_size);
		}

        DynamicArray(DynamicArray<value_t>&& other)
        {
			m_data = other.m_data;
			m_size = other.m_size;
			m_capacity = other.m_capacity;
			new (&other) DynamicArray<value_t>();
        }

		DynamicArray(size_t capacity)
		{
			m_capacity = capacity;
			m_data = (value_t*)malloc(m_capacity * elementSize);
			m_size = 0;
		}

		DynamicArray(value_t* ptr, size_t lenght)
		{
			m_data = ptr;
			m_size = lenght;
			m_capacity = lenght;
		}

		DynamicArray(size_t lenght, value_t filler)
		{
			m_size = lenght;
			m_capacity = lenght;
			m_data = malloc(m_capacity  * elementSize);
			fill(filler);
		}

		template<class argT> DynamicArray(InitList<argT> data)
		{
			m_size = data.size();
			m_capacity = m_size;
			m_data = (value_t*)malloc(m_capacity * elementSize);

			value_t* thisIterator = begin();
			value_t* thisEnd = end();
			const argT* dataIterator = data.begin();

			while(thisIterator < thisEnd)
			{
				*thisIterator = danikk_framework::move(value_t(*dataIterator));
				thisIterator++;
				dataIterator++;
			}
		}

		DynamicArray<value_t>& operator=(const DynamicArray<value_t>& other)
		{
			resize(other.size());
			for(index_t i = 0; i < size(); i++)
			{
				m_data[i] = other.m_data[i];
			}
			return *this;
		}

		DynamicArray<value_t>& operator=(DynamicArray<value_t>&& other)
		{
			if(m_data != NULL)
			{
				free(m_data);
			}
			m_data = other.m_data;
			m_size = other.m_size;
			m_capacity = other.m_capacity;
			other.m_data = NULL;
			other.m_size = 0;
			other.m_capacity = 0;
			return *this;
		}

		~DynamicArray()
		{
            if (m_data != NULL)
			{
				free(m_data);
				m_data = NULL;
			}
			m_size = 0;
		}

		bool operator!=(const DynamicArray<value_t>& other) const
        {
            if(other.size() != m_size)
            {
                return true;
            }

            for(size_t index = 0; index < m_size; index++)
            {
                if(other[index] != m_data[index])
                {
                    return true;
                }
            }
            return false;
        }

        bool operator==(const DynamicArray<value_t>& other) const
        {
            if(other.size() != m_size)
            {
                return false;
            }

            for(size_t index = 0; index < m_size; index++)
            {
                if(other[index] != m_data[index])
                {
                    return false;
                }
            }
            return true;
        }

		void cutElements(value_t element)
		{
			size_t cuttedElementsCount = 0;
			value_t* endElementPtr = end();
			for(value_t* currentElementPtr = begin();currentElementPtr < endElementPtr; currentElementPtr++)
			{
				*(currentElementPtr - cuttedElementsCount) = *currentElementPtr;
				if(*currentElementPtr == element)
				{
					cuttedElementsCount++;
				}
			}
		}

		void fill(value_t filler)
		{
			value_t* endElementPtr = end();
			for(value_t* ptr = begin(); ptr < endElementPtr; ptr++)
			{
				*ptr = filler;
			}
		}

		void clear()
		{
			for(value_t& current : *this)
			{
				(&current)->~value_t();
			}
			m_size = 0;
		}

		void reservesize(size_t count)
		{
			if(m_capacity >= count)												//Если вместимости хватает.
			{
				return;															//То ничего не делает.
			}
			else if(m_data != NULL && m_capacity != 0)
			{
				while(m_capacity < count)										//Если память была выделена до этого
				{
					m_capacity *= 2;
				}
				m_data = (value_t*)(*&realloc)(m_data, m_capacity * elementSize);//то "расширяет" её в 2 раза, пока необходимая вместимость не будет достигнута.
				return;
			}
			else
			{
				m_capacity = 1;
				if(count < 2048)
				{
					while(m_capacity < count)
					{
						m_capacity *= 2;
					}
				}
				else
				{
					m_capacity = count;
				}
				m_data = (value_t*)malloc(elementSize * m_capacity);
			}
		}

		void reserve(size_t count)
		{
			reservesize(count + m_size);
		}

		//Изменяет размер массива, не вызывает деструкторы/конструкторы
		void rawResize(size_t newsize)
		{
			reservesize(newsize);
			m_size = newsize;
		}

		void resize(size_t newsize)
		{
			if(newsize == size())
			{
				return;
			}
			else if(size() == 0)
			{
				reservesize(newsize);
				m_size = newsize;
			}
			else if(newsize > size())
			{
				reservesize(newsize);
				m_size = newsize;
			}
			else
			{
				while(newsize < m_size)
				{
					m_size--;
					(&m_data[m_size])->~value_t();
				}
			}
		}

		//Изменяет размер массива, инициализируя элементы заданным значением.
		void resize(size_t newsize, const value_t& baseElement)
		{
			if(newsize == size())
			{
				return;
			}
			else if(size() == 0)
			{
				reservesize(newsize);
				m_size = newsize;
				for(value_t& value : *this)
				{
					value = baseElement;
				}
			}
			else if(newsize > size())
			{
				index_t index = size();
				reservesize(newsize);
				m_size = newsize;

				while(index < size())
				{
					m_data[index] = baseElement;
					index++;
				}
			}
			else
			{
				while(newsize < m_size)
				{
					m_size--;
					(&m_data[m_size])->~value_t();
				}
			}
		}

		//Все push функции возвращают ссылку на добавленный элемент в массиве.

		value_t& push(const value_t& newElement)
		{
			reserve(1);
			value_t& result = m_data[m_size];
			m_size++;
			result = newElement;
			return result;
		}

		value_t& pushCtor()
		{
			reserve(1);
			value_t& result = m_data[m_size];
			m_size++;
			new (&result) value_t();
			return result;
		}

		template<class... args_t> value_t& pushCtor(const args_t&... args)
		{
			reserve(1);
			value_t& result = m_data[m_size];
			m_size++;
			new (&result) value_t(args...);
			return result;
		}

		template<class... args_t> value_t& pushCtor(args_t&... args)
		{
			reserve(1);
			value_t& result = m_data[m_size];
			m_size++;
			new (&result) value_t(args...);
			return result;
		}

		value_t& pushMove(value_t&& newElement)
		{
			reserve(1);
			value_t& result = m_data[m_size];
			m_size++;
			new (&result) value_t(move(newElement));
			return result;
		}

		value_t& pushMove(value_t& newElement)
		{
			return pushMove(move(newElement));
		}

		value_t& operator[](const size_t index)
		{
			return m_data[index];
		}

		const value_t& operator[](const size_t index) const
		{
			return m_data[index];
		}

		value_t& peek() const
		{
			return m_data[m_size - 1];
		}

		value_t pop()
		{
			m_size--;
			return move(m_data[m_size]);
		}

		value_t qpop(size_t index)
		{
			value_t result;
			swap(m_data[index], result);
			swap(m_data[--m_size], m_data[index]);
			return move(result);
		}

		value_t* data()
		{
			return m_data;
		}

		value_t* data() const
		{
			return m_data;
		}

		value_t* begin()
		{
			return m_data;
		}

		const value_t* begin() const
		{
			return m_data;
		}

		value_t* end()
		{
			return m_data + m_size;
		}

		const value_t* end() const
		{
			return m_data + m_size;
		}

		size_t capacity() const
		{
			return m_capacity;
		}

		size_t size() const
		{
			return m_size;
		}

		bool willFit(size_t index) const
		{
			return index < m_size;
		}

		bool willFit(int index) const
		{
			return index < (int)m_size && index > 0;
		}

		value_t pull(value_t& element_ref)
		{
			value_t result = move(element_ref);
			value_t* element_ptr = &element_ref;
			assert(element_ptr < begin() || element_ptr >= end());
			if(element_ptr == end() - 1)
			{
				pop();
			}
			else
			{
				element_ref = move(pop());
			}
			return move(result);
		}

		//перемещает элемент element_ref из текущего массива в dest.
		void moveTo(value_t& element_ref, type& dest)
		{
			dest.pushMove(pull(element_ref));
		}

		//Удаляет элемент по ссылке, которая указывает на элемент массива.
		void quickErase(value_t& element_ref)
		{
			value_t* element_ptr = &element_ref;
			assert((ptrdiff_t)element_ptr >= (ptrdiff_t)begin() || (ptrdiff_t)element_ptr < (ptrdiff_t)end());

			if(element_ptr == end() - 1)
			{
				pop();
			}
			else
			{
				*element_ptr = pop();
			}
		}

		//Удаляет все элементы массива, которые подходят под условие.
		void quickEraseWhere(auto find_func)
		{
			size_t index = 0;
			while(true)
			{
				if(index >= m_size || m_size == 0)
				{
					return;
				}
				while(find_func(m_data[index]))
				{
					m_data[index] = move(pop());
				}
				index++;
			}
		}

		//Возвращает ссылку на первый совпадающий элемент в массиве.
		value_t* find(const value_t& arg_value)
		{
			for(value_t& value : *this)
			{
				if(arg_value == value)
				{
					return &value;
				}
			}
			return NULL;
		}

		value_t* findWhere(auto find_func)
		{
			for(value_t& value : *this)
			{
				if(find_func(value))
				{
					return &value;
				}
			}
			return NULL;
		}

		int64 indexOf(const value_t& arg_value)
		{
			for(value_t& value : *this)
			{
				if(arg_value == value)
				{
					return &value - m_data;
				}
			}
			return -1;
		}

		int64 lastIndexOf(const value_t& arg_value)
		{
			value_t* current_ptr = this->end() - 1;
			value_t* end_ptr = this->begin() - 1;
			while(current_ptr < end_ptr)
			{
				if(arg_value == *current_ptr)
				{
					return (int64)(current_ptr - m_data);
				}
				current_ptr--;
			}
			return -1;
		}

		int64 indexWhere(auto find_func)
		{
			for(value_t& value : *this)
			{
				if(find_func(value))
				{
					return &value - m_data;
				}
			}
			return -1;
		}

		bool contains(const value_t& value) const
		{
			for(const value_t& current : *this)
			{
				if(current == value)
				{
					return true;
				}
			}
			return false;
		}

		bool containsWhere(auto find_func) const
		{
			for(const value_t& value : *this)
			{
				if(find_func(value))
				{
					return true;
				}
			}
			return false;
		}

		DynamicArray<value_t*> selectRef(auto where)
		{
			DynamicArray<value_t*> result;

			for(value_t& value : *this)
			{
				if(where(value))
				{
					result.push(&value);
				}
			}

			return result;
			//return move(result);
		}

		DynamicArray<value_t> select(auto where) const
		{
			DynamicArray<value_t> result;

			for(const value_t& value : *this)
			{
				if(where(value))
				{
					result.push(value);
				}
			}

			return move(result);
		}

		template<class field_t> size_t getUniqueCountBy(auto get_field_func)
		{
			DynamicArray<const field_t&> array;

			for(const value_t& value : *this)
			{
				field_t* field = &get_field_func(value);

				for(const field_t& arr_value : array)
				{
					if(*arr_value == *field)
					{
						goto iter_end;
					}
				}
				array.push(field);
				iter_end:
				continue;
			}

			return array.size();
		}

		value_t& last()
		{
			return m_data[m_size - 1];
		}

		bool empty() const
		{
			return m_size == 0;
		}
	};

	template<class stream_t, class value_t> stream_t& operator << (stream_t& stream, const DynamicArray<value_t>& array)
	{
		for(index_t i = 0; i < array.size(); i++)
		{
			stream << (const value_t&)array[i];
			if(i < array.size() - 1)
			{
				stream << ' ';
			}
		}
		return stream;
	}
}
