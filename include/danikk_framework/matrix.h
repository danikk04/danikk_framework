#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/matrix_iter.h>
#include <danikk_framework/log.h>
#include <danikk_framework/glm.h>
#include <danikk_framework/assert.h>

namespace danikk_framework
{
	template<class value_type, uvec2 m_size> class FixedMatrix
	{
	public:
		typedef value_type value_t;
		typedef FixedMatrix<value_type, m_size> this_t;
	private:
		static constexpr size_t data_size = m_size.x * m_size.y;
		value_type m_data[data_size];
	public:
		value_t& operator[](uvec2 index)
		{

			assert(index.x < m_size.x);
			assert(index.y < m_size.y);

			return m_data[index.x + index.y * m_size.x];
		}
		value_t& operator[](index_t index)
		{
			assert(index < data_size);
			return m_data[index];
		}

		FixedMatrix& operator =(const this_t& other)
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

		uvec2 size() const
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

	template<class value_type> class FixedRefMatrix
	{
	public:
		typedef value_type value_t;
	private:
		value_t* data = NULL;
		uvec2 m_size{0};
	public:
		FixedRefMatrix() = default;

		FixedRefMatrix(value_t* data, size_t size)
		{
			this->data = data;
			this->m_size = uvec2(size, size);
		}

		FixedRefMatrix(value_t* data, uint32 size_x, uint32 size_y)
		{
			this->data = data;
			this->m_size = uvec2(size_x, size_y);
		}

		template<class index_t> value_t& operator[](index_t index)
		{
			assert(data != NULL);
			assert(index.x < m_size.x);
			assert(index.y < m_size.y);

			return data[index.x + index.y * m_size.x];
		}

		value_t& at(uint32 x, uint32 y)
		{
			assert(data != NULL);
			assert(x < m_size.x);
			assert(y < m_size.y);

			return data[x + y * m_size.x];
		}

		uvec2 size()
		{
			return m_size;
		}

		void setRow(const value_t& data, size_t x)
		{
			for(size_t y = 0; y < m_size.y; y++)
			{
				this->at(x, y) = data;
			}
		}

		void setColumn(const value_t& data, size_t y)
		{
			for(size_t x = 0; x < m_size.x; x++)
			{
				this->at(x, y) = data;
			}
		}

		uvec2 size() const
		{
			return m_size;
		}
	};

	template<class value_type> class SubMatrix
	{
	public:
		typedef value_type value_t;
	private:
		FixedRefMatrix<value_t>* matrix;
		uvec2 m_start;
		uvec2 m_size;
	public:

		SubMatrix(FixedRefMatrix<value_t>& matrix,
				uint32 start_x, uint32 start_y, uint32 size_x, uint32 size_y)
		{
			this->matrix = &matrix;
			this->m_start = uvec2(start_x, start_y);
			this->m_size = uvec2(size_x, size_y);
		}

		/*const ref<value_t> at(uint32 x, uint32 y) const
		{
			assert(matrix != NULL);
			assert(x < m_size.x);
			assert(y < m_size.y);

			return matrix->at(m_start.x + x, m_start.y + y);
		}

		ref<value_t> at(uint32 x, uint32 y)
		{
			assert(matrix != NULL);
			assert(x < m_size.x);
			assert(y < m_size.y);

			return matrix->at(m_start.x + x, m_start.y + y);
		}

		const ref<value_t> at(uvec2 index) const
		{
			assert(matrix != NULL);
			assert(index.x < m_size.x);
			assert(index.y < m_size.y);

			return matrix->at(m_start.x + index.x, m_start.y + index.y);
		}

		ref<value_t> at(uvec2 index)
		{
			assert(matrix != NULL);
			assert(index.x < m_size.x);
			assert(index.y < m_size.y);

			return matrix[uvec2(m_start.x + index.x, m_start.y + index.y)];
		}*/

		template<class index_t> value_t& operator[](index_t index)
		{
			assert(matrix != NULL);
			assert(index.x < m_size.x);
			assert(index.y < m_size.y);

			return (*matrix)[uvec2(m_start.x + index.x, m_start.y + index.y)];
		}

		uvec2 size() const
		{
			return m_size;
		}

		void move(uint32 x, uint32 y)
		{
			m_start = uvec2(x, y);
		}
	};

	namespace matrix_functions
	{
		template<class bitmap_input, class bitmap_output> void scaleCopy(bitmap_input& input, bitmap_output& output)
		{
			uvec2 inputSize = input.size();
			uvec2 outputSize = output.size();

			vec2 scaleFactor =  (vec2)inputSize / (vec2)outputSize;

			for(uvec2 current = uvec2(0);
					isMatrixIteration(current, outputSize);
					incrementMatrixIndex(current, outputSize))
			{
				uvec2 inputIndex = uvec2((uint32)(current.x * scaleFactor.x), (uint32)(current.y * scaleFactor.y));
				output[current] = input[inputIndex];
			}
		}
	}
}
