#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/assert.h>

#include <string>
#include <cstring>
#include <cstdlib>

namespace danikk_framework
{
	class GenericString
	{
	public:
		char& operator [](size_t index)
		{
			return data()[index];
		}

		const char& operator [](size_t index) const
		{
			return data()[index];
		}

		void clear()
		{
			resize(0);
		}

	    char* c_string()
	    {
	    	reserve(1);
	        data()[size()] = '\0';
	        return data();
	    }

		char first() const
		{
			return *data();
		}

		char last() const
		{
			return *(end() - 1);
		}

		GenericString& operator << (char data)
		{
			reserve(1);
			*end() = data;
			addsize(1);
			return *this;
		}

		GenericString& operator << (const char* data)
		{
			return append(data, strlen(data));
		}

		GenericString& operator << (const GenericString& str)
		{
			return append(str.data(), str.size());
		}

	    GenericString& operator =(const char* str);

    	GenericString& operator=(const GenericString& other);

    	GenericString& operator <<(float value);

    	GenericString& operator <<(uint64 value);

    	void copyData(const GenericString& source)
    	{
    		copyData(source.data(), source.size());
    	}

    	void copyData(const char* source)
    	{
    		copyData(source, strlen(source));
    	}

    	void copyData(const char* source, const char* endptr)
    	{
    		copyData(source, endptr - source);
    	}

    	void copyData(const char* source, size_t sourceSize)
    	{
    		resize(sourceSize);
    		memcpy(data(), source, sourceSize);
    	}

	    bool operator !=(const GenericString& other) const;

	    bool operator ==(const GenericString& other) const;

	    bool operator ==(const char* other) const;

	    bool operator >(const GenericString& other) const;

	    bool operator <(const GenericString& other) const;

		GenericString& cutChars(char chr);

        GenericString& substring(const char* begin, const char* end);

        GenericString& cutAfterSeparator(char separator);

        GenericString& cutBeforeSeparator(char separator);

        GenericString& cutAfterLastSeparator(char separator);

        GenericString& cutBeforeLastSeparator(char separator);

        GenericString& cutSideChars(char chr);

        GenericString& eraseCharacters(const GenericString& targets);

        GenericString& replace(const GenericString& sub, const GenericString& result);

        GenericString& appendToStart(const GenericString& str);

        GenericString& append(const char* data, size_t size);

	    char* find(char character)
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

	    const char* find(char character) const
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

		char* findLast(char character)
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

		const char* findLast(char character) const
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

	    void split(const char* splitPtr, GenericString& output1, GenericString& output2) const
	    {
	    	assert((ptrdiff_t)splitPtr > (ptrdiff_t)begin() && (ptrdiff_t)splitPtr < (ptrdiff_t)end());
	    	output1.clear();
	    	output2.clear();
	        output1.resize((size_t)splitPtr - (size_t)begin());
	        output2.resize((size_t)end() - (size_t)splitPtr - 1);
	        memcpy(output2.data(), splitPtr + 1, output2.size());
	        memcpy(output1.data(), data(), output1.size());
	    }

		int indexOf(char chr) const
		{
			index_t res = 0;
			while(data()[res] != chr || res++ != size());
			if(res == size())
			{
				return -1;
			}
			return res;
		}

        bool haveEqualChar(const GenericString& otherString) const;

        bool haveOnlyEqualChars(const GenericString& otherString) const;

        bool contains(char character) const;

        bool startsWith(const char* str) const;

        void splitToFirstSeparator(char separator, GenericString& output1, GenericString& output2) const;

        void splitToLastSeparator(char separator, GenericString& output1, GenericString& output2) const;

		virtual const char* data() const = 0;
		virtual const char* begin() const = 0;
		virtual const char* end() const = 0;
		virtual char* data() = 0;
		virtual char* begin() = 0;
		virtual char* end() = 0;
		virtual size_t size() const = 0;
		virtual size_t capacity() const = 0;
		virtual void resize(size_t new_size) = 0;
		virtual void addsize(size_t plus_size) = 0;
		virtual void substractsize(size_t minus_size) = 0;
		virtual void reserve(size_t) = 0;
	};
}
