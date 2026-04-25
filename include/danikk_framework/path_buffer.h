#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/string.h>
#include <danikk_framework/current_system.h>
#include <danikk_framework/cstring_functions.h>
#include <danikk_framework/number.h>

namespace danikk_framework
{
#ifdef IS_LINUX
    constexpr char pathSlash = '/';
#elif IS_WINDOWS
    constexpr char pathSlash = '\\';
#endif

    //Класс для работы с путями.
    class PathBuffer
    {
    private:
        static constexpr size_t defaultCapacity = 512 - sizeof(char*);

        char m_data[defaultCapacity];
        char* m_abp;

        void findabp();

        void push(const char* data, size_t size);

        void initEmpty();
    public:

        PathBuffer();

        PathBuffer(const char* path);

        PathBuffer(const String& path);

        PathBuffer(const PathBuffer& path);

        PathBuffer& operator=(const PathBuffer& path);

        PathBuffer& operator << (char chr);

        PathBuffer& setPath(const char* path);

        PathBuffer& setFileName(const char* name);

        PathBuffer& setFileName(const String& name, const String& extension);

        template<class stringT> PathBuffer& setFileName(const stringT& name);

        template<class dirT> PathBuffer& pushDirectory(const dirT& name);

    	PathBuffer& popDirectory();

        PathBuffer& getcwd();

    	PathBuffer& getDesktopDirectory();

    	PathBuffer& getExecutableDirectory();

    	const char* getLastDirectoryName();

        const char* c_string() const;

        void skip(size_t offset);

        bool isEmpty() const;

        char* abp();

        char* data();

        const char* data() const;

        size_t size() const;

        size_t capacity() const;

        char& operator[](size_t index);

		char first() const;

		char last() const;

	    friend const PathBuffer& getcwd();

	    friend const PathBuffer& getDesktopDirectory();

	    friend const PathBuffer& getExecutableDirectory();
    };

    template<class stream_t> stream_t& operator <<(stream_t& cout, const PathBuffer& data)
    {
    	cout << data.c_string();
    	return cout;
    }

    template<class stringT> PathBuffer& PathBuffer::pushDirectory(const stringT& name)
    {
    	if constexpr(is_cstring<stringT>::value)
    	{
    		const char* namePtr = name;

    		if(*namePtr == pathSlash && last() == pathSlash)
    		{
        		*this << (namePtr + 1);
    		}
    		else
    		{
    			*this << (namePtr);
    		}
    		char abpm1 = *(m_abp - 1);
    		//char abpm2 = *(m_abp - 1);
    		if(abpm1 != pathSlash)
    		{
    			*this << (pathSlash);
    		}
    		/*if(abpm1 == pathSlash && abpm2 == pathSlash)
    		{
    			abpm1--;
    		}*/
    		*m_abp = '\0';
    	}
    	else
    	{
    		if(name.first() == pathSlash && last() == pathSlash)
    		{
				push(name.data() + 1, name.size() - 1);
    			if(name.last() != pathSlash)
        		{
    				*this << pathSlash;
        		}
    		}
    		else
    		{
    			push(name.data(), name.size());

    			if(name.last() != pathSlash)
        		{
        			*this << pathSlash;
        		}
    		}
        	*m_abp = '\0';
    	}
        return *this;
    }

    template<class stringT> PathBuffer& PathBuffer::setFileName(const stringT& name)
    {
		char* tempabp = m_abp;
		size_t nameSize = name.size();
		memcpy(tempabp, name.data(), nameSize);
		tempabp += name.size();
		*tempabp = '\0';

		return *this;
    }

    const PathBuffer& getcwd();

    const PathBuffer& getDesktopDirectory();

    const PathBuffer& getExecutableDirectory();
}
