#include <danikk_framework/string.h>
#include <danikk_framework/localization.h>
#include <danikk_framework/array.h>
#include <danikk_framework/cstring_functions.h>
#include <danikk_framework/number.h>
#include <danikk_framework/assert.h>
#include <danikk_framework/number.h>
#include <stdlib.h>
#include <string>

using namespace std;

namespace danikk_framework
{
	bool stringToBool(const String& str)
	{
		if (str == "1")
		{
			return true;
		}
		else if(str == "0")
		{
			return false;
		}
		else if(str == "True")
		{
			return true;
		}
		else if(str == "False")
		{
			return false;
		}
		else if(str == "true")
		{
			return true;
		}
		else if(str == "false")
		{
			return false;
		}
		else
		{
			return false;
		}
	}

	const char* boolToNumberString(bool value)
	{
		return value ? "1" : "0";
	}

	//static const char* hexdict = "123456789ABCDEF";

	/*String hex(const String& data)
	{
		String result;
		byte* cur = (byte*)data.begin();
		byte* end = (byte*)data.end();
		while(cur < end)
		{
			byte chr = *cur;
			cur++;
			result << hexdict[chr & 0xF0] << hexdict[chr & 0x0F];
		}
		return move(result);
	}

	String unhex(const String& data)
	{
		String result;
		byte* cur = (byte*)data.begin();
		byte* end = (byte*)data.end();
		while(cur < end)
		{
			byte hex1 = *cur;
			cur++;
			byte hex2 = *cur;
			cur++;
			byte chr =
		}
		return move(result);
	}*/

	String toCodeList(const String& data)
	{
		String result;
		const byte* cur = (const byte*)data.begin();
		const byte* end = (const byte*)data.end();
		while(cur < end)
		{
			byte chr = *cur;
			cur++;
			result << (uint64)chr << ';';
		}
		return move(result);
	}

	String unCodeList(const String& data)
	{
		String result;
		const char* cur = (const char*)data.begin();
		const char* end = (const char*)data.end();
		String numbuf;
		while(cur < end)
		{
			char chr = *cur;
			if(chr == ';')
			{
				byte value = parseNumber<byte>(numbuf);
				result << *((char*)&value);
				numbuf.clear();
			}
			else
			{
				numbuf << chr;
			}
			cur++;
		}
		return move(result);
	}

	void toCodeList(String& data)
	{
		byte* cdata = (byte*)alloca(data.size());
		const byte* cur = cdata;
		const byte* end = cdata + data.size();
		memcpy(cdata, data.data(), data.size());
		data.resize(0);
		while(cur < end)
		{
			byte chr = *cur;
			cur++;
			data << (uint64)chr << ';';
		}
	}

	void unCodeList(String& data)
	{
		const char* cur = (const char*)data.begin();
		const char* end = (const char*)data.end();
		data.resize(0);
		String numbuf;
		while(cur < end)
		{
			char chr = *cur;
			if(chr == ';')
			{
				byte value = parseNumber<byte>(numbuf);
				data << *((char*)&value);
				numbuf.clear();
			}
			else
			{
				numbuf << chr;
			}
			cur++;
		}
	}

	String::String()
	{
    	m_data = 0;
    	m_size = 0;
    	m_capacity = 0;
	}

    String::String(const String& other)
    {
        m_size = other.m_size;
        m_capacity = m_size + 1;
		m_data = (char*)malloc(m_capacity);
		memcpy(m_data, other.m_data, m_size);
    }

	String::String(const std::string& value):String((const char*)value.data(), value.size())
	{

	}

    String::String(String&& other)
    {
    	assert((ptrdiff_t)this != (ptrdiff_t)&other);
		m_data = other.m_data;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		new (&other) String();
    }

	String::String(const char* ptr, size_t size)
	{
        m_size = size;
        m_capacity = m_size + 1;
		m_data = (char*)malloc(m_capacity);
		memcpy(m_data, ptr, m_size);
    	m_data[m_size] = '\0';
	}

	String::String(const char* ptr)
	{
		if(ptr != NULL)
		{
			new (this) String(ptr, strlen(ptr));
		}
		else
		{
			new (this) String("", (size_t)0);
		}
	}

	String::String(const char* begin, const char* end):String(begin, (ptrdiff_t)end - (ptrdiff_t)begin)
	{

	}

	String::String(char* ptr):String(ptr, strlen(ptr))
	{

	}

	String::String(char* ptr, size_t size):String(ptr, size, size + 1)
	{

	}

    String::String(char* ptr, size_t size, size_t capacity)
    {
    	m_data = ptr;
    	m_size = size;
    	m_capacity = capacity;
    }

    String::String(size_t size)
	{
    	m_size = size;
    	m_capacity = size + 1;
    	m_data = (char*)malloc(m_capacity);
    	m_data[m_size] = '\0';
	}

	String::String(size_t size, char initChar)
	{
    	m_size = size;
    	m_capacity = size + 1;
    	m_data = (char*)malloc(m_capacity);
    	memset(m_data, initChar, m_size);
    	m_data[m_size] = '\0';
	}

	String::~String()
	{
		if(m_data != NULL)
		{
			free(m_data);
			m_data = NULL;
		}
	}

    void String::reserve(size_t count)
	{
		size_t target_capacity = count + m_size + 1;
		if(m_capacity >= target_capacity)							//Если вместимости хватает.
		{
			return;													//То ничего не делает.
		}

		if(m_data != NULL)
		{
			if(target_capacity < 16)
			{
				m_capacity = 16;
			}
			else
			{
				while(m_capacity < target_capacity)
				{
					m_capacity <<= 1;
				}
			}

			//m_data = (char*)realloc(m_data, m_capacity);

			char* new_data = (char*)malloc(m_capacity);
			memcpy(new_data, m_data, m_size);
			free(m_data);
			m_data = new_data;
		}
		else
		{
			m_capacity = 1;
			while(m_capacity < target_capacity)
			{
				m_capacity <<= 1;
			}
			m_data = (char*)malloc(m_capacity);
		}
	}
}
