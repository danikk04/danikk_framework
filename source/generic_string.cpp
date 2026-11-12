#include <danikk_framework/generic_string.h>
#include <danikk_framework/number.h>

#include <cstdint>
#include <cstdlib>
#include <limits>

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
		const size_t old_size = size();
		const size_t sub_size = sub.size();
		const size_t result_size = result.size();
		if(sub_size == 0 || sub_size > old_size)
		{
			return *this;
		}

		const char* original = data();
		const char* search = sub.data();
		const char* replacement = result.data();
		size_t match_count = 0;
		for(size_t offset = 0; offset + sub_size <= old_size;)
		{
			if(memcmp(original + offset, search, sub_size) == 0)
			{
				++match_count;
				offset += sub_size;
			}
			else
			{
				++offset;
			}
		}
		if(match_count == 0)
		{
			return *this;
		}

		size_t new_size = old_size;
		if(result_size >= sub_size)
		{
			const size_t growth_per_match = result_size - sub_size;
			if(growth_per_match != 0 && match_count > (std::numeric_limits<size_t>::max() - old_size) / growth_per_match)
			{
				std::abort();
			}
			new_size += match_count * growth_per_match;
		}
		else
		{
			new_size -= match_count * (sub_size - result_size);
		}
		if(new_size == std::numeric_limits<size_t>::max())
		{
			std::abort();
		}

		char* rebuilt = (char*)malloc(new_size + 1);
		if(rebuilt == nullptr) std::abort();
		size_t read_offset = 0;
		size_t write_offset = 0;
		while(read_offset < old_size)
		{
			if(read_offset + sub_size <= old_size
				&& memcmp(original + read_offset, search, sub_size) == 0)
			{
				if(result_size != 0)
				{
					memcpy(rebuilt + write_offset, replacement, result_size);
				}
				write_offset += result_size;
				read_offset += sub_size;
			}
			else
			{
				rebuilt[write_offset++] = original[read_offset++];
			}
		}
		rebuilt[new_size] = '\0';
		resize(new_size);
		if(new_size != 0)
		{
			memcpy(data(), rebuilt, new_size);
		}
		data()[new_size] = '\0';
		free(rebuilt);
		return *this;
	}

	GenericString& GenericString::appendToStart(const GenericString& str)
	{
		const size_t original_size = size();
		const size_t prefix_size = str.size();
		if(prefix_size > std::numeric_limits<size_t>::max() - original_size)
		{
			std::abort();
		}
		char* prefix = (char*)malloc(prefix_size == 0 ? 1 : prefix_size);
		if(prefix == nullptr) std::abort();
		if(prefix_size != 0) memcpy(prefix, str.data(), prefix_size);
		resize(size() + prefix_size);
		memmove(data() + prefix_size, data(), original_size);
		if(prefix_size != 0) memcpy(data(), prefix, prefix_size);
		free(prefix);
		return *this;
	}

    GenericString& GenericString::append(const char* data_ptr, size_t data_size)
    {
		const size_t old_size = size();
		if(data_size > std::numeric_limits<size_t>::max() - old_size)
		{
			std::abort();
		}
		const uintptr_t data_address = reinterpret_cast<uintptr_t>(data());
		const uintptr_t source_address = reinterpret_cast<uintptr_t>(data_ptr);
		const bool source_aliases_this = source_address >= data_address
			&& source_address - data_address <= old_size;
		const size_t source_offset = source_aliases_this ? source_address - data_address : 0;
		resize(size() + data_size);
		if(source_aliases_this)
		{
			data_ptr = data() + source_offset;
		}
		if(data_size != 0)
		{
			memmove(data() + old_size, data_ptr, data_size);
		}
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
