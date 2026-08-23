#pragma once

#include <danikk_framework/dynamic_array.h>

namespace danikk_framework
{
	template <class value_t> class VirtualDynamicArray
	{
	private:
		DynamicArray<value_t*>* data = NULL;

		void checkExits()
		{
			if(data == NULL)
			{
				void* ptr = malloc(sizeof(DynamicArray<value_t*>));
				data = new (ptr) DynamicArray<value_t*>();
			}
		}
	public:
		VirtualDynamicArray() = default;

		value_t& push(const value_t& newElement)
		{
			checkExits();
			data->push(newElement);
		}

		value_t* begin()//если внутри массива нет, то не создаёт его, а просто возвращает указатель на самого себя
		{
			if(data == NULL)
			{
				return (value_t*)this;
			}
			else
			{
				return data->begin();
			}
		}

		value_t* end()
		{
			if(data == NULL)
			{
				return (value_t*)this;
			}
			else
			{
				return data->end();
			}
		}
	};
}
