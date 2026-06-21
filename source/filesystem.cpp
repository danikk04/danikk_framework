#include <danikk_framework/filesystem.h>

using namespace std;

namespace danikk_framework
{
	static const char* systemTempDirectory = "/tmp/";

	static PathBuffer tempDirectory;

	const char* getTempDirectory()
	{
		return tempDirectory.c_string();
	}

	void getFullNameFromPath(const char* path, size_t size, String& result)
	{
		const char* end = path + size;
		const char* afterLastPathSlash = end;
		//Тут end переводим из '\0';
		end--;
		//Тут, если на конце стоит pathSlash, то end сдвигает.
		//Это нужно для директорий, так-как на конце может быть косая черта.
		if(*end == pathSlash)
		{
			end--;
		}
		while(*afterLastPathSlash != pathSlash)
		{
			afterLastPathSlash--;
		}
		result.copyData(afterLastPathSlash + 1, end);
	}

	void getFullNameFromPath(const char* path, String& result)
	{
		return getFullNameFromPath(path, strlen(path), result);
	}

	void getFullNameFromPath(const String& path, String& result)
	{
		return getFullNameFromPath(path.data(), path.size(), result);
	}

	static inline void getNameFromPathPtrs(const char* lastPointPtr, const char* lastPathSlashPtr, const char* end, String& result)
	{
		if(lastPathSlashPtr + 1 == end)
		{
			lastPathSlashPtr--;
			while(*lastPathSlashPtr != pathSlash)
			{
				--lastPathSlashPtr;
			}
		}

		if(lastPointPtr < lastPathSlashPtr)//если у файла нет расширения или у родительской директории есть расширение
		{
			result.copyData(lastPathSlashPtr + 1, end);
		}
		else
		{
			result.copyData(lastPathSlashPtr + 1, lastPointPtr - 1);
		}
	}

	void getNameFromPath(const char* path, String& result)
	{
		const char* lastPointPtr = NULL;
		const char* lastPathSlashPtr = NULL;
		const char* end = NULL;

		for(const char* current = path; true; current++)
		{
			char currentChar = *current;
			switch(currentChar)
			{
			case '.':
				lastPointPtr = current;
				break;
			case pathSlash:
				lastPathSlashPtr = current;
				break;
			case '\0':
				end = current;
				goto iter_end;
			}
		}
		iter_end:
		getNameFromPathPtrs(lastPointPtr, lastPathSlashPtr, end, result);
	}

	void getNameFromPath(const String& path, String& result)
	{
		const char* lastPointPtr = path.findLast('.');
		const char* lastPathSlashPtr = path.findLast(pathSlash);
		const char* end = result.end();

		getNameFromPathPtrs(lastPointPtr, lastPathSlashPtr, end, result);
	}

	const char* getExtensionFromPath(const char* path)
	{
		const char* lastPathSlashPtr = strrchr(path, pathSlash);
		const char* lastPointPtr = strrchr(lastPathSlashPtr, '.');//Что-бы не итерироваться по path второй раз можно просто искать точку в lastPathSlashPtr.

		if(lastPointPtr == NULL)
		{
			return lastPathSlashPtr + 1;
		}
		else
		{
			if(lastPointPtr < lastPathSlashPtr)//Если у директории есть расширение.
			{
				return lastPathSlashPtr + 1;
			}
			else
			{
				return lastPointPtr + 1;
			}
		}
	}

	/*const char* generateTempPath()
	{
#ifdef IS_LINUX
		static StringBuffer<128> tempPath;

		static size_t counter = 0;

		while(true)
		{
			tempPath << tempDirectory.c_string() << numberToString(counter++);
			if(exits(tempPath.c_string()))
			{
				tempPath.clear();
			}
			else
			{
				break;
			}
		}
#else
#error not implemented
#endif
		return String((const char*)tempPath.data(), tempPath.size());
	}*/

	static void atexitTempDirectory()
	{
		remove(getTempDirectory());
	}

	void initTempDirectory(const char* name)
	{
		atexit(&atexitTempDirectory);

		//Тут создаётся папка для всех временных файлов приложения.
		//Если папки с названием приложения нет, то создаёт
		//её и использует.
		//Если она есть в конец добавляет номер.
		tempDirectory.pushDirectory(systemTempDirectory);
		tempDirectory.pushDirectory(name);
		if(exitsFile(tempDirectory))
		{
			size_t lastNumberSize = 0;
			char* tempDirectoryEnd = tempDirectory.abp();//МБ тут ошибка будет.
			*tempDirectoryEnd++ = '_';
			size_t tempPathCounter = 0;
			do
			{
				lastNumberSize = writeNumberToStringBuffer(tempPathCounter++, tempDirectoryEnd, 64);
			}
			while(exitsFile(tempDirectory));
			tempDirectory.skip(lastNumberSize + 1);
		}
		createDirectory(tempDirectory);
	}

	void cwdToExd()
	{
		chdir(getExecutableDirectory().c_string());
	}
}
