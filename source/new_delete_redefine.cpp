#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/memory.h>
#include <danikk_framework/log.h>

#define MALLOC_LOG 1

#undef malloc
#undef realloc

namespace danikk_framework
{
	bool enable_memory_log = false;

	void* malloc(size_t s)
	{
		if (enable_memory_log)
		{
			formatLogInfo("malloc size:%", s);
		}
		return ::malloc(s);
	}

	void* realloc(void* ptr, size_t s)
	{
		if (enable_memory_log)
		{
			formatLogInfo("realloc size:%", s);
		}
		return ::realloc(ptr, s);
	}

	void* malloc_debug(const char* message, size_t s)
	{
		if (enable_memory_log)
		{
			formatLogInfo("% malloc size:%", message, s);
		}
		return ::malloc(s);
	}

	void* realloc_debug(const char* message, void* ptr, size_t s)
	{
		if (enable_memory_log)
		{
			formatLogInfo("% realloc size:%", message, s);
		}
		return ::realloc(ptr, s);
	}
}

/*void* operator new (size_t size) {
    return danikk_framework::malloc(size);
}

void operator delete(void* ptr, size_t)
{
	free(ptr);
}*/
