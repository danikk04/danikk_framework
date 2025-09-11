#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/glm.h>
#include <danikk_framework/matrix_iter.h>
#include <danikk_framework/log.h>
#include <danikk_framework/assert.h>

namespace danikk_framework
{
	class DynamicTensorPosIterator
	{
	private:
		uvec3 current = uvec3(0);
		uvec3 m_size;
	public:
		DynamicTensorPosIterator() = default;

		DynamicTensorPosIterator(uvec3 current, uvec3 size) : current(current), m_size(size){}

		bool operator!=(const DynamicTensorPosIterator& other)
		{
			return current.z != other.current.z;
		}

		uvec3 operator*()
		{
			return current;
		}

		DynamicTensorPosIterator& operator++()
		{
			current.x++;
			if(current.x == m_size.x)
			{
				current.y++;
				current.x = 0;
			}
			if(current.y == m_size.y)
			{
				current.z++;
				current.y = 0;
			}
			assert(current.z != m_size.z + 1);
			return *this;
		}
	};

	template<uvec3 m_size> class TensorPosIterator
	{
	private:
		uvec3 current = uvec3(0);
	public:
		TensorPosIterator() = default;

		TensorPosIterator(uvec3 current) : current(current){}

		bool operator!=(const TensorPosIterator<m_size> & other) const = default;
		bool operator==(const TensorPosIterator<m_size> & other) const = default;

		uvec3 operator*()
		{
			return current;
		}

		TensorPosIterator<m_size>& operator++()
		{
			current.x++;
			if(current.x == m_size.x)
			{
				current.y++;
				current.x = 0;
			}
			if(current.y == m_size.y)
			{
				current.z++;
				current.y = 0;
			}
			assert(current.z != m_size.z + 1);
			return *this;
		}
	};

	template<uvec3 m_size> class TensorIterable
	{
	public:
		TensorPosIterator<m_size> begin()
		{
			return TensorPosIterator<m_size>();
		}

		TensorPosIterator<m_size> end()
		{
			return TensorPosIterator<m_size>(uvec3(0, 0, m_size.z));
		}
	};

	template<class value_type, uvec3 m_size> class FixedTensor
	{
	public:
		typedef value_type value_t;
		typedef FixedTensor<value_type, m_size> this_t;
	private:
		static constexpr size_t data_size = m_size.x * m_size.y * m_size.z;
		value_type m_data[data_size];
	public:

		TensorIterable<m_size> iteratePos()
		{
			return TensorIterable<m_size>();
		}

		inline bool isValidIndex(const uvec3& index)
		{
			return index.x < m_size.x && index.y < m_size.y && index.z < m_size.z;
		}

		value_t& operator[](uvec3 index)
		{
			assert(index.x < m_size.x);
			assert(index.y < m_size.y);
			assert(index.z < m_size.z);

			return m_data[index.x +
						  index.y * m_size.x +
						  index.z * m_size.x * m_size.y];
		}
		value_t& operator[](index_t index)
		{
			assert(index < data_size);
			return m_data[index];
		}

		this_t& operator =(const this_t& other)
		{
			for(index_t i = 0; i < data_size; i ++)
			{
				this->m_data[i] = other.m_data[i];
			}
			return *this;
		}

		bool operator ==(const this_t& other) const
		{
			for(index_t i = 0; i < data_size; i ++)
			{
				if(this->m_data[i] != other.m_data[i])
				{
					return false;
				}
			}
			return true;
		}

		uvec3 size() const
		{
			return m_size;
		}

		value_type* begin()
		{
			return m_data;
		}

		value_type* end()
		{
			return m_data + data_size;
		}

		value_type* begin() const
		{
			return m_data;
		}

		value_type* end() const
		{
			return m_data + data_size;
		}
	};
}
