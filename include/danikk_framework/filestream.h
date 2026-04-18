#pragma once

#include <danikk_framework/string.h>
#include <danikk_framework/number.h>
#include <danikk_framework/errno.h>
#include <danikk_framework/static_string.h>
#include <danikk_framework/path_buffer.h>
#include <danikk_framework/exception.h>
#include <filesystem>

namespace danikk_framework
{
    //Общий класс для файловых потоков.

#define DEF_OPEN_F template<class path_t> void open(path_t& path, const char* flag)
#define DEF_OPEN template<class path_t> void open(path_t& path)

    class FileStream
    {
    protected:
        FILE* filePtr = NULL;
        String path;
        const char* flag = NULL;
    	size_t m_size = 0;

    	void open();
    public:
    	FileStream(FILE* filePtr)
    	{
    		this->filePtr = filePtr;
    	}

    	DEF_OPEN_F
        {
        	const char* c_path = tocstring(path);
        	if(c_path != NULL)
        	{
        		this->flag = flag;
            	this->path = String(c_path);
            	open();
        	}
        }

        void checkSize();

        //Собщает позицию в файле
        long tell();

        //Переместить позицию файла.
        void seek(long offset, int mode);

        //Переместить позицию файла на указанную позицию.
        void set(long offset);

        //Сдвинуть указатель относительно текущего положения потока.
        void move(long offset);

        void end();

        void resize(uint64 new_size);

        int eof();

        bool isOpen();

        void close();

        int flush();

        size_t size();

        FileStream() = default;

        ~FileStream();

        FileStream(const FileStream&) = delete;

        void operator=(const FileStream&) = delete;

        template <class valueT> size_t write(const valueT* source, size_t size = 1)
        {
        	return fwrite(source, size, sizeof(valueT), filePtr);
        }

        size_t writeNull(size_t size);

        template <class valueT> FileStream& operator<<(const valueT& input)
        {
        	write((const char*)&input, sizeof(valueT));
        	return *this;
        }

        template <class valueT> size_t read(valueT* destination, size_t size = 1)
        {
        	return fread(destination, 1, size * sizeof(valueT), filePtr);
        }

        template <class valueT> FileStream& operator>>(valueT& output)
        {
        	fread(&output, 1, sizeof(valueT), filePtr);
        	return *this;
        }
    };

    //Поток чтения из файла.
    class FileReader : public FileStream
    {
    public:
    	DEF_OPEN
        {
        	FileStream::open(path, "r");
        }

        FileReader() = default;
    };

    //Поток записи в файл.
    class FileWriter : public FileStream
    {
    public:
    	DEF_OPEN
        {
        	FileStream::open(path, "w");
        }

        FileWriter() = default;
    };
}
