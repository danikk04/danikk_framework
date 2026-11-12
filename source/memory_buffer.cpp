#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/memory_buffer.h>
#include <danikk_framework/memory.h>
#include <stdlib.h>
#include <cstring>

namespace danikk_framework
{
    MemoryBuffer::~MemoryBuffer()
    {
        if(m_data!=NULL)
        {
            ::free(m_data);
        }
    }

	MemoryBuffer::MemoryBuffer(size_t capacity)
	{
		m_data = (char*)malloc(capacity);
		m_size = 0;
		m_capacity = capacity;
	}

    void MemoryBuffer::reserve(size_t reservecapacity)
    {
    	size_t target_capacity = reservecapacity + m_size;
        if (target_capacity <= m_capacity)
        {
        	return;
        }
        else if(m_data != NULL)
		{
			while(m_capacity < target_capacity)
			{
				m_capacity *= 2;
			}
			m_data = (char*)realloc(m_data, m_capacity);
		}
		else
		{
			m_capacity = 1;
			while(m_capacity < target_capacity)
			{
				m_capacity *= 2;
			}
			m_data = (char*)malloc(m_capacity);
		}
    }

    void MemoryBuffer::resize(size_t dataSize)
    {
        reserve(dataSize);
        m_size = dataSize;
    }

    void MemoryBuffer::clear()
	{
		m_size = 0;
	}

    void MemoryBuffer::skip(size_t size)
    {
    	m_size += size;
    }

    char* MemoryBuffer::c_string()
    {
    	reserve(1);
        m_data[m_size] = '\0';
        return m_data;
    }

    char* MemoryBuffer::data()
    {
    	return m_data;
    }

    size_t MemoryBuffer::size()
    {
    	return m_size;
    }

    size_t MemoryBuffer::capacity()
    {
    	return m_capacity;
    }

    void MemoryBuffer::moveTo(String& data)
    {
		if(m_size == 0) data.clear();
		else data.assign(m_data, m_size);
		::free(m_data);
		m_data = nullptr;
		m_size = 0;
		m_capacity = 0;
    }
}
