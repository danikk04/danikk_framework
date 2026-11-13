#pragma once

#include <cstring>
#include <cstdlib>
#include <danikk_framework/danikk_framework.h>

namespace danikk_framework
{
	inline constexpr size_t kib(size_t v)
	{
		return v << 10;
	}

	inline constexpr size_t mib(size_t v)
	{
		return v << 20;
	}

	inline constexpr size_t gib(size_t v)
	{
		return v << 30;
	}

	template<class valueT> inline void objectToDefault(valueT& ref)
	{
		ref->~valueT();
		new (&ref) valueT();
	}

#undef malloc
#undef realloc

	void enableAllocatorLog();

	void* malloc(size_t s);

	void* realloc(void* ptr, size_t s);

	void* malloc_debug(const char* message, size_t s);

	void* realloc_debug(const char* message, void* ptr, size_t s);

	// Returns the process resident memory size in bytes, or 0 when unavailable.
	size_t getProcessMemoryUsageBytes();

#if IS_DEBUG
#define malloc(arg) danikk_framework::malloc_debug(__PRETTY_FUNCTION__, arg);
#define realloc(arg, size) danikk_framework::realloc_debug(__PRETTY_FUNCTION__, arg, size);
#elif
#define malloc(arg) danikk_framework::malloc(arg);
#define realloc(arg, size) danikk_framework::realloc(arg, size);
#endif

	template<class valueT> inline valueT* structalloc()
	{
		return (valueT*)malloc(sizeof(valueT));
	}

	/*inline uint64 memorytonum(byte* input, size_t s)
	{
		uint64 res = 0;
		uint64 factor = 1;
		for(char* end = input + s; input < end; input++)
		{
			res += (uint64)(*input) * factor;
			factor <<= 8;
		}
		return res;
	}*/
}
