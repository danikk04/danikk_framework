#pragma once

#include <iostream>
#include <type_traits>

#include <danikk_framework/danikk_framework.h>
#include "cstring_functions.h"

namespace danikk_framework
{
	bool stringToBool(const String& str);

	const char* boolToNumberString(bool value);

	/*String hex(cref<String> data);

	String unhex(cref<String> data);*/

	//String toCodeList(cref<String> data);

	//String unCodeList(cref<String> data);

	//void toCodeList(ref<String> data);

	//void unCodeList(ref<String> data);

    class Utf8Iterator
    {
    private:
        char* charPtr;
        size_t octetSize;
    public:
        Utf8Iterator(char* ptr);

        bool operator!=(const Utf8Iterator& other) const;

        bool operator==(const Utf8Iterator& other) const;

        size_t operator-(const Utf8Iterator& other) const;

        operator char*();

        operator void*();

        void operator++();

        const Utf8Iterator operator++(int);

        unicode_char operator*() const;
    };

    class String
    {
    	friend class MemoryBuffer;
    private:
        mutable char* m_data;//В c_string нужно ставить нуль терминатор в конец строки.
        mutable size_t m_size;
        mutable size_t m_capacity;

        void pushMemory(const char* memory, size_t size);
    public:
        String();

        String(const String& other);

        String(const std::string& other);

        String(String&& other);

		String(const char* ptr);

		String(const char* begin, const char* end);

		String(const char* ptr, size_t size);

		String(char* ptr);

		String(char* ptr, size_t size);

		String(char* ptr, size_t size, size_t capacity);

		String(size_t size);

		String(size_t size, char initChar);

		~String();

	    operator const std::string&() const
		{
	    	return *((const std::string*)this);
		}

		String copy() const;

		void copyData(const String& source);

		void copyData(const char* source);

		void copyData(const char* source, size_t sourceSize);

		void copyData(const char* source, const char* endptr);

	    bool operator !=(const String& other) const;

	    bool operator ==(const String& other) const;

	    bool operator ==(const char* other) const;

	    bool operator >(const String& other) const;

	    bool operator <(const String& other) const;

	    operator bool() const;

	    char& operator[](size_t index) const;

	    String& operator =(const String& other);

	    String& operator =(const char* str);

	    String& operator =(String&& other);

	    String& operator <<(char value);

	    String& operator <<(uint64 value);

	    String& operator <<(float value);

		size_t size() const;

	    const char* data() const;

		char* data();

		const char* begin() const;

		char* begin();

		const char* end() const;

		char* end();

		char* c_string();

		const char* c_string() const;

        char* find(char character);

        const char* find(char character) const;

        char* findLast(char character);

        const char* findLast(char character) const;

		int indexOf(char chr) const;

        void clear();

		void reserve(size_t count) const;

		void push(const char* data, size_t dataSize);

		void push(const String& otherString);

        void push(char character);

        void resize(size_t count);

        void nullifyMemory();

        char firstChar() const;

        char lastChar() const;

        bool haveEqualChar(const String& otherString) const;

        bool haveOnlyEqualChars(const String& otherString) const;

        bool empty() const;

		void split(const char* splitPlace, String& output1, String& output2) const;

        bool contains(char character) const;

        bool startsWith(const char* str) const;

        void splitToFirstSeparator(char separator, String& output1, String& output2) const;

        void splitToLastSeparator(char separator, String& output1, String& output2) const;

        String& cutChars(char chr);

        String& substring(const char* begin, const char* end);

        String& cutAfterSeparator(char separator);

        String& cutBeforeSeparator(char separator);

        String& cutAfterLastSeparator(char separator);

        String& cutBeforeLastSeparator(char separator);

        String& cutSideChars(char chr);

        String& replace(const String& sub, const String& result);

        String& appendToStart(const String& str);

        String& append(const char* data, size_t size);

        String& operator <<(const String& other);
    };
}
