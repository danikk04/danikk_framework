#pragma once

#include <danikk_framework/cstring_functions.h>
#include <danikk_framework/current_system.h>
#include <danikk_framework/dynamic_array.h>
#include <danikk_framework/filestream.h>
#include <danikk_framework/number.h>
#include <danikk_framework/path_buffer.h>
#include <danikk_framework/static_string.h>
#include <danikk_framework/string.h>
#include <danikk_framework/current_system.h>
#include <danikk_framework/errno.h>
#include <danikk_framework/memory.h>

#include <filesystem>
#include "memory_buffer.h"

#if IS_LINUX
	#include <sys/stat.h>
	#include <unistd.h>
#elif IS_WINDOWS
	#include <direct.h>
#else
	#error not implemented
#endif

namespace danikk_framework
{
	template<class pathT> void readLines(const pathT& path, DynamicArray<String>& output)
	{
        FileReader reader;
        reader.open(path);
		DynamicArray<char> data;
		uint fileSize = reader.size();
		data.resize(fileSize);
		reader.read(data.data(), fileSize);
		data.cutElements('\r');
		char* currentCharPtr = data.data();
		char* endPtr = data.end();
		const char* lastStringStartPtr = currentCharPtr;
		output.clear();

		while(currentCharPtr < endPtr)
		{
            char currentChar = *currentCharPtr;
            if(currentChar == '\n')
            {
                *currentCharPtr = '\0';
            	output.pushCtor(lastStringStartPtr, (size_t)currentCharPtr - (size_t)lastStringStartPtr);
            	lastStringStartPtr = (currentCharPtr) + 1;
            }
            ++currentCharPtr;
		}
	}

	template<class pathT> bool exitsFile(pathT& path)
	{
		int result = ::access(tocstring(path), F_OK);
		errno = 0;
		return result == 0;
	}

	template<class pathT> errno_t createDirectory(pathT& path)
	{
		#ifdef IS_LINUX
			ERRNO_CALL(int ret = mkdir(tocstring(path), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ));
		#else
			#error not implemented
		#endif
		return ret;
	}

	template<class pathT> uint createFile(pathT& path)
	{
		ERRNO_CALL(FILE* filePtr = fopen(tocstring(path), "w"));
		return filePtr != NULL;
	}

	template<class pathT> errno_t removeFile(const pathT& path)
	{
		ERRNO_CALL(remove(tocstring(path)));
		return 0;
	}

	template<class firststr, class secondstr> errno_t renameFile(firststr& oldPath, secondstr& newPath)
	{
		ERRNO_CALL(rename(tocstring(oldPath), tocstring(newPath)));
		return 0;
	}

	template<class pathT> bool isDirectory(pathT& path)
	{
		#if IS_LINUX
			struct stat st;
			stat(tocstring(path), &st);
			return S_ISDIR(st.st_mode);
		#else
			#error not implemented
			return 0;
		#endif
	}

	template<class pathT, class size_t> int fileSize(pathT& path, size_t& size)
	{
		struct stat st;
		ERRNO_CALL(stat(tocstring(path), &st));
		size = st.st_size;
		if(size == 0)
		{
			size = std::filesystem::file_size(path);
		}
		return 0;
	}

	template<class pathT, class size_t> size_t getCharCount(pathT& path, char target)
	{
		FileReader reader;
		reader.open(path);
		size_t buff_cap = kib(8);
		size_t buff_size = buff_cap;
		char buffer[buff_cap];
		int counter = 0;
		while(!reader.eof())
		{
			buff_size = reader.read(buffer, buff_cap);
			for(index_t i = 0; i < buff_size; i++)
			{
				if(buffer[i] == target)
				{
					counter++;
				}
			}
		}
		return counter;
	}

	//Получает путь к папке со всеми временными файлами приложения.
	const char* getTempDirectory();

	//Возвращяет имя с расширением.
	void getFullNameFromPath(const char* path, String& result);

	void getFullNameFromPath(const String& path, String& result);

	//Возвращает имя без расширения.
	void getNameFromPath(const char* path, String& result);

	void getNameFromPath(const String& path, String& result);

	const char* getExtensionFromPath(const char* path);

	//Инициализирует директорию с временными файлами.
	void initTempDirectory(const char* name);

	//CWD переносит в директорию с исполняемым файлом
	void cwdToExd();
}
