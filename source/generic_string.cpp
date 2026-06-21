#include <danikk_framework/internal/generic_string.h>
#include <danikk_framework/number.h>

using namespace std;

namespace danikk_framework
{
    bool GenericString::operator!=(const GenericString& other) const
	{
    	if(other.size() != size())
    	{
    		return true;
    	}
    	const char* otherData = other.data();
    	for(size_t index = 0; index < size(); index++)
    	{
    		if(data()[index] != otherData[index])
    		{
    			return true;
    		}

    	}
    	return false;
	}

    bool GenericString::operator==(const GenericString& other) const
	{
    	if(other.size() != size())
    	{
    		return false;
    	}
    	const char* otherData = other.data();
    	for(size_t index = 0; index < size(); index++)
    	{
    		if(data()[index] != otherData[index])
    		{
    			return false;
    		}
    	}
    	return true;
	}

    bool GenericString::operator ==(const char* other) const
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

	GenericString& GenericString::cutChars(char chr)
	{
		char* readptr = data();
		char* writeptr = data();
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

		resize(writeptr - data() - 1);

		return *this;
	}

	GenericString& GenericString::substring(const char* begin, const char* end)
	{
		size_t m_size = end - begin;
		memmove(data(), begin, m_size);
		resize(m_size);
		return *this;
	}

	GenericString& GenericString::cutAfterSeparator(char separator)
	{
		substring(find(separator), end());
		return *this;
	}

	GenericString& GenericString::cutBeforeSeparator(char separator)
	{
		substring(begin(), find(separator));
		return *this;
	}

	GenericString& GenericString::cutAfterLastSeparator(char separator)
	{
		substring(findLast(separator), end());
		return *this;
	}

	GenericString& GenericString::cutBeforeLastSeparator(char separator)
	{
		substring(begin(), findLast(separator));
		return *this;
	}

	GenericString& GenericString::cutSideChars(char chr)
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

	GenericString& GenericString::eraseCharacters(const GenericString& targets)
	{
		char* currentWritePtr = data();
		char* currentReadPtr = data();
		char* endPtr = end();

		while(currentReadPtr != endPtr)
		{
			char chr = *currentReadPtr;
			if(!targets.contains(chr))
			{
				*currentWritePtr = *currentReadPtr;
				currentWritePtr++;
			}
			currentReadPtr++;
		}
		resize(currentWritePtr - data());
		return *this;
	}

	GenericString& GenericString::replace(const GenericString& sub, const GenericString& result)
	{
		size_t resultSize = result.size();
		size_t subSize = sub.size();

		if(resultSize < subSize)
		{
			char* endPtr = end();
			char* currentWritePtr = data();
			char* currentReadPtr = data();
			size_t deltaSize = sub.size() - result.size();

			while(currentReadPtr != endPtr)
			{
				if(strstartswith(currentReadPtr, sub))
				{
					memcpy(currentWritePtr, result.data(), resultSize);
					currentWritePtr += resultSize;
					currentReadPtr += subSize;
					addsize(deltaSize);
				}
				else
				{
					*currentWritePtr = *currentReadPtr;//ХЗ
					currentWritePtr++;
					currentReadPtr++;
				}
			}
		}
		else if(resultSize == subSize)
		{
			char* endPtr = end();
			char* currentPtr = data();

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
				char* currentPtr = data();

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

			addsize(sizeGrow);

			//Вся строка будет перемещена во временный стековый буффер
			char* tempStringCopy = (char*)alloca(size());
			memcpy(tempStringCopy, data(), size());
			char* endPtr = tempStringCopy + size();
			char* writePtr = data();
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

	GenericString& GenericString::appendToStart(const GenericString& str)
	{
		uint32 original_size = size();
		uint32 append_size = str.size();
		addsize(append_size);
		memmove(data() + append_size, data(), original_size);
		memcpy(data(), str.data(), append_size);
		return *this;
	}

    GenericString& GenericString::append(const char* data_ptr, size_t data_size)
    {
    	reserve(data_size);
		memcpy(end(), data_ptr, data_size);
		addsize(data_size);
		return *this;
    }

	GenericString& GenericString::operator =(const GenericString& other)
	{
        resize(other.size());
		memcpy(data(), other.data(), size());
        return *this;
	}

	GenericString& GenericString::operator =(const char* str)
	{
		this->clear();
		*this << str;
		return *this;
	}

	GenericString& GenericString::operator <<(float value)
    {
    	char buffer[64];
    	writeNumberToStringBuffer(value, buffer, 64);
    	*this << buffer;
    	return *this;
    }

	GenericString& GenericString::operator <<(uint64 value)
    {
    	char buffer[64];
    	writeNumberToStringBuffer(value, buffer, 64);
    	*this << buffer;
    	return *this;
    }

    bool GenericString::haveEqualChar(const GenericString& otherString) const
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

    bool GenericString::haveOnlyEqualChars(const GenericString& otherString) const
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

    bool GenericString::contains(char character) const
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

    bool GenericString::startsWith(const char* str) const
    {
    	index_t i;
    	for(i = 0; i < size(); i++)
    	{
    		char chr1 = str[i];
    		char chr2 = data()[i];
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

    void GenericString::splitToFirstSeparator(char separator, GenericString& output1, GenericString& output2) const
    {
    	split(find(separator), output1, output2);
    }

    void GenericString::splitToLastSeparator(char separator, GenericString& output1, GenericString& output2) const
    {
    	split(findLast(separator), output1, output2);
    }
}
