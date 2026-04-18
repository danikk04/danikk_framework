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

	String::String(const std::string& value):String((const char*)value.data(), value.size()){}

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

	String::String(const char* begin, const char* end):String(begin, (ptrdiff_t)end - (ptrdiff_t)begin){}

	String::String(char* ptr):String(ptr, strlen(ptr)){}

	String::String(char* ptr, size_t size):String(ptr, size, size + 1){}

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

	String String::copy() const
	{
		return String(*this);
	}

	void String::copyData(const String& source)
	{
		copyData(source.data(), source.size());
	}

	void String::copyData(const char* source)
	{
		copyData(source, strlen(source));
	}

	void String::copyData(const char* source, size_t sourceSize)
	{
		resize(sourceSize);
		memcpy(m_data, source, sourceSize);
	}

	void String::copyData(const char* source, const char* endptr)
	{
		copyData(source, endptr - source);
	}

    bool String::operator!=(const String& other) const
	{
    	if(other.m_size != m_size)
    	{
    		return true;
    	}
    	char* otherData = other.m_data;
    	for(size_t index = 0; index < m_size; index++)
    	{
    		if(m_data[index] != otherData[index])
    		{
    			return true;
    		}

    	}
    	return false;
	}

    bool String::operator==(const String& other) const
	{
    	if(other.size() != m_size)
    	{
    		return false;
    	}
    	char* otherData = other.m_data;
    	for(size_t index = 0; index < m_size; index++)
    	{
    		if(m_data[index] != otherData[index])
    		{
    			return false;
    		}
    	}
    	return true;
	}

    bool String::operator ==(const char* other) const
	{
    	const char* current = begin();
    	const char* endPtr = end();

    	while(current < endPtr)
    	{
    		char chr1 = *current;
    		char chr2 = *other;

    		if(chr1 != chr2)
    		{
    			return false;
    		}
    		current++;
    		other++;
    	}
    	if(*other != '\0')
    	{
    		return false;
    	}
    	return true;
	}

    String::operator bool() const
	{
    	return size() > 0;
	}

	String& String::operator =(const String& other)
	{
        m_size = other.m_size;
        m_capacity = m_size;
		m_data = (char*)malloc(m_size);
		memcpy(m_data, other.m_data, m_size);
        return *this;
	}

	String& String::operator =(const char* str)
	{
		this->clear();
		*this << str;
		return *this;
	}

	String& String::operator = (String&& other)
	{
		m_data = other.m_data;
		other.m_data = NULL;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
        return *this;
	}

	char& String::operator[](size_t index) const
	{
		return m_data[index];
	}

    String& String::operator << (char value)
    {
    	reserve(1);
    	m_data[m_size] = value;
    	m_size++;
        return *this;
    }

    String& String::operator <<(uint64 value)
    {
    	char buffer[64];
    	writeNumberToStringBuffer(value, buffer, 64);
    	*this << buffer;
    	return *this;
    }

    String& String::operator <<(float value)
    {
    	char buffer[64];
    	writeNumberToStringBuffer(value, buffer, 64);
    	*this << buffer;
    	return *this;
    }

    void String::pushMemory(const char* memory, size_t size)
    {
    	reserve(size);
    	memcpy(m_data + m_size, memory, size);
    	m_size += size;
    }

	size_t String::size() const
	{
		return m_size;
	}

    char* String::data()
	{
		return m_data;
	}

	const char* String::data() const
	{
		return (const char*)m_data;
	}

	char* String::begin()
    {
        return m_data;
    }

	const char* String::begin() const
    {
        return m_data;
    }

	char* String::end()
    {
        return m_data + m_size;
    }

	const char* String::end() const
    {
        return m_data + m_size;
    }

    char* String::c_string()
    {
    	assert(m_data != NULL);
    	reserve(1);
        m_data[m_size] = '\0';
        return m_data;
    }

    const char* String::c_string() const
    {
        if(m_data != NULL)
        {
        	reserve(1);
            m_data[m_size] = '\0';
            return m_data;
        }
        return "";
    }

    bool String::haveEqualChar(const String& otherString) const
    {
    	const char* thisEnd = end();
    	const char* otherEnd = otherString.end();
        for (const char* thisIterator= begin(); thisIterator < thisEnd; thisIterator++)
        {
            for (const char* otherIterator = otherString.begin(); otherIterator < otherEnd; otherIterator++)
            {
                if (*thisIterator == *otherIterator)
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool String::haveOnlyEqualChars(const String & otherString) const
    {
    	const char* thisEnd = end();
    	const char* otherEnd = otherString.end();
        for (const char* thisIterator = begin(); thisIterator != thisEnd; thisIterator++)
        {
        	bool exits = false;
            for (const char* otherIterator = otherString.begin(); otherIterator != otherEnd; otherIterator++)
            {
                if (*thisIterator == *otherIterator)
                {
                	exits = true;
                }
            }
            if(!exits)
            {
            	return false;
            }
        }
        return true;
    }

    bool String::empty() const
    {
    	return m_size == 0;
    }

    void String::split(const char* splitPtr, String& output1, String& output2) const
    {
    	assert((ptrdiff_t)splitPtr > (ptrdiff_t)begin() && (ptrdiff_t)splitPtr < (ptrdiff_t)end());
    	output1.clear();
    	output2.clear();
        output1.resize((size_t)splitPtr - (size_t)begin());
        output2.resize((size_t)end() - (size_t)splitPtr - 1);
        memcpy(output2.data(), splitPtr + 1, output2.size());
        memcpy(output1.data(), data(), output1.size());
    }

    char* String::find(char character)
    {
		char* stringEnd = end();
        for (char* currentCharPtr = begin(); currentCharPtr < stringEnd; currentCharPtr++)
        {
            if (character == *currentCharPtr)
            {
                return currentCharPtr;
            }
        }
        return NULL;
    }

    const char* String::find(char character) const
    {
		const char* stringEnd = end();
        for (const char* currentCharPtr = begin(); currentCharPtr < stringEnd; currentCharPtr++)
        {
            if (character == *currentCharPtr)
            {
                return currentCharPtr;
            }
        }
        return NULL;
    }

	char* String::findLast(char character)
    {
        char* stringEnd = begin();
        for(char* currentCharPtr = end(); currentCharPtr != stringEnd; currentCharPtr--)
        {
        	if(*currentCharPtr == character)
        	{
        		return currentCharPtr;
        	}
        }
        return NULL;
    }

	const char* String::findLast(char character) const
    {
        const char* stringEnd = begin();
        for(const char* currentCharPtr = end(); currentCharPtr != stringEnd; currentCharPtr--)
        {
        	if(*currentCharPtr == character)
        	{
        		return currentCharPtr;
        	}
        }
        return NULL;
    }

	int String::indexOf(char chr) const
	{
		index_t res = 0;
		while(m_data[res] != chr || res++ != m_size);
		if(res == m_size)
		{
			return -1;
		}
		return res;
	}

    void String::clear()
    {
        m_size = 0;
    }

    void String::reserve(size_t count) const
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

    void String::push(const char* data, size_t dataSize)
    {
        reserve(dataSize);
        memcpy(m_data + m_size, data, dataSize);
        m_size += dataSize;
    }

    void String::push(const String& otherString)
    {
        push(otherString.m_data, otherString.m_size);
    }

    void String::push(char character)
    {
		reserve(1);
		m_data[m_size++] = character;
    }

    void String::resize(size_t new_size)
    {
    	if(new_size > m_size)
    	{
    		reserve(new_size + 1);
    	}
		m_size = new_size;
    }

    void String::nullifyMemory()
    {
    	memset(m_data, 0, m_capacity);
    	/*char* end = m_data + m_capacity;
    	for(char* ptr = m_data; ptr < end; ptr++)
    	{
    		*ptr = '\0';
    	}*/
    }

    char String::firstChar() const
    {
    	return *m_data;
    }

    char String::lastChar() const
    {
    	return *(m_data + m_size - 1);
    }

    bool String::contains(char character) const
    {
    	const char* stringEnd = end();
        for (const char* currentIterator = begin(); currentIterator != stringEnd; currentIterator++)
        {
            if (character == *currentIterator)
            {
                return true;
            }
        }
        return false;
    }

    bool String::startsWith(const char* str) const
    {
    	index_t i;
    	for(i = 0; i < size(); i++)
    	{
    		char chr1 = str[i];
    		char chr2 = m_data[i];
    		if(chr1 == '\0')
    		{
    			return true;
    		}
    		if(chr1 != chr2)
    		{
    			return false;
    		}
    	}
    	return *(str + i) == '\0';
    }

    void String::splitToFirstSeparator(char separator, String& output1, String& output2) const
    {
    	split(find(separator), output1, output2);
    }

    void String::splitToLastSeparator(char separator, String& output1, String& output2) const
    {
    	split(findLast(separator), output1, output2);
    }

    String& String::cutChars(char chr)
    {
		char* readptr = m_data;
		char* writeptr = m_data;
		char* endptr = end();
		char curchr;

		while(readptr != endptr)
		{
			curchr = *readptr++;
			if(curchr != chr)
			{
				*writeptr++ = curchr;
			}
		}

		m_size = writeptr - m_data - 1;

    	return *this;
    }

    String& String::substring(const char* begin, const char* end)
    {
    	size_t m_size = end - begin;
    	memmove(m_data, begin, m_size);
		return *this;
    }

    String& String::cutAfterSeparator(char separator)
    {
    	substring(find(separator), end());
		return *this;
    }

    String& String::cutBeforeSeparator(char separator)
    {
    	substring(begin(), find(separator));
		return *this;
    }

    String& String::cutAfterLastSeparator(char separator)
    {
        substring(findLast(separator), end());
		return *this;
    }

    String& String::cutBeforeLastSeparator(char separator)
    {
        substring(begin(), findLast(separator));
		return *this;
    }

    String& String::cutSideChars(char chr)
    {
    	char* newData = begin();
    	char* newEnd = end();

    	while(*newData == chr)
    	{
    		newData++;
    	}

    	while(*newEnd == chr)
    	{
    		newEnd--;
    	}

    	substring(newData, newEnd);
		return *this;
    }

    String& String::replace(const String& sub, const String& result)
    {
    	size_t resultSize = result.size();
    	size_t subSize = sub.size();

    	if(resultSize < subSize)
    	{
    		char* endPtr = end();
    		char* currentWritePtr = m_data;
    		char* currentReadPtr = m_data;
    		size_t deltaSize = sub.size() - result.size();

    		while(currentReadPtr != endPtr)
    		{
    			if(strstartswith(currentReadPtr, sub))
    			{
    				memcpy(currentWritePtr, result.data(), resultSize);
    				currentWritePtr += resultSize;
    				currentReadPtr += subSize;
    				m_size -= deltaSize;
    			}
    			else
    			{
    				*currentWritePtr = *currentReadPtr;
    				currentWritePtr++;
    				currentReadPtr++;
    			}
    		}
    	}
    	else if(resultSize == subSize)
    	{
    		char* endPtr = end();
    		char* currentPtr = m_data;

    		while(currentPtr != endPtr)
    		{
    			if(strstartswith(currentPtr, sub))
    			{
    				memcpy(currentPtr, result.data(), resultSize);
    				currentPtr += resultSize;
    			}
    		}
    	}
    	else
    	{
    		size_t subCount = 0;

    		{
        		char* endPtr = end();
        		char* currentPtr = m_data;

        		while(currentPtr < endPtr)
        		{
        			if(strstartswith(currentPtr, sub))
        			{
        				currentPtr += subSize;
        				subCount++;
        			}
        			else
        			{
        				currentPtr++;
        			}
        		}
    		}

    		size_t deltaSize = resultSize - subSize;
    		size_t sizeGrow = subCount * deltaSize;

    		resize(m_size + sizeGrow);

    		//Вся строка будет перемещена во временный стековый буффер
    		char* tempStringCopy = (char*)alloca(m_size);
    		memcpy(tempStringCopy, m_data, m_size);
    		char* endPtr = tempStringCopy + m_size;
    		char* writePtr = m_data;
    		char* readPtr = tempStringCopy;

    		while(readPtr < endPtr)
    		{
    			if(strstartswith(readPtr, sub))
    			{
    				memcpy(writePtr, result.data(), resultSize);
    				readPtr += subSize;
    				writePtr += resultSize;
    			}
    			else
    			{
    				*writePtr++ = *readPtr++;
    			}
    		}
    	}

		return *this;
    }

    String& String::appendToStart(const String& str)
    {
    	uint32 original_size = size();
    	uint32 append_size = str.size();
    	resize(original_size + append_size);
    	memmove(m_data + append_size, m_data, original_size);
    	memcpy(m_data, str.data(), append_size);
    	return *this;
    }

    String& String::append(const char* data, size_t size)
    {
    	resize(this->size() + size);
    	memcpy(end() - size, data, size);
    	return *this;
    }

    String& String::operator <<(const String& other)
    {
    	reserve(other.size());
    	for(char chr : other)
    	{
    		m_data[m_size++] = chr;
    	}
    	/*char* write_ptr = end();
    	resize(size() + other.size());
    	memcpy(write_ptr, other.data(), other.size());*///ПОЧЕМУТО НЕ РАБОТАЕТ
    	return *this;
    }
}
