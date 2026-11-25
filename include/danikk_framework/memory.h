#pragma once

#include <cstring>
#include <cstdlib>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <danikk_framework/danikk_framework.h>

namespace danikk_framework
{
	using MemoryTypeId = size_t;

	/** @brief Current and peak dynamic-array storage attributed to one element type. */
	struct MemoryTypeStats
	{
		MemoryTypeId type_id = 0;
		std::string type_name;
		size_t active_allocations = 0;
		size_t reserved_objects = 0;
		size_t reserved_bytes = 0;
		size_t peak_reserved_bytes = 0;
		size_t total_allocated_bytes = 0;
	};

	/** @brief Adds one typed allocation to the current memory totals. */
	void registerMemoryAlloc(MemoryTypeId type_id, const char* type_name, size_t object_count, size_t bytes);
	/** @brief Removes one typed allocation from the current memory totals. */
	void registerMemoryDealloc(MemoryTypeId type_id, const char* type_name, size_t object_count, size_t bytes);
	/** @brief Replaces a typed allocation's old object and byte counts with its new counts. */
	void registerMemoryRealloc(MemoryTypeId type_id, const char* type_name, size_t old_object_count, size_t old_bytes, size_t new_object_count, size_t new_bytes);
	/** @brief Logs up to max_entry element types with the largest currently reserved memory totals. */
	void logMemoryUse(uint32 max_entry = 40);

	template<class value_t> inline const char* getMemoryTypeName()
	{
		if constexpr(std::is_same_v<value_t, int8>) { return "int8"; }
		else if constexpr(std::is_same_v<value_t, uint8>) { return "uint8"; }
		else if constexpr(std::is_same_v<value_t, int16>) { return "int16"; }
		else if constexpr(std::is_same_v<value_t, uint16>) { return "uint16"; }
		else if constexpr(std::is_same_v<value_t, int32>) { return "int32"; }
		else if constexpr(std::is_same_v<value_t, uint32>) { return "uint32"; }
		else if constexpr(std::is_same_v<value_t, int64>) { return "int64"; }
		else if constexpr(std::is_same_v<value_t, uint64>) { return "uint64"; }
		else if constexpr(std::is_same_v<value_t, float>) { return "float"; }
		else if constexpr(std::is_same_v<value_t, double>) { return "double"; }
		else { return typeid(value_t).name(); }
	}

	#define DSE_REGISTER_MEMORY_ALLOC(type, object_count) \
		::danikk_framework::registerMemoryAlloc(typeid(type).hash_code(), ::danikk_framework::getMemoryTypeName<type>(), object_count, sizeof(type) * object_count)
	#define DSE_REGISTER_MEMORY_DEALLOC(type, object_count) \
		::danikk_framework::registerMemoryDealloc(typeid(type).hash_code(), ::danikk_framework::getMemoryTypeName<type>(), object_count, sizeof(type) * object_count)
	#define DSE_REGISTER_MEMORY_REALLOC(type, old_object_count, old_bytes, new_object_count, new_bytes) \
		::danikk_framework::registerMemoryRealloc(typeid(type).hash_code(), ::danikk_framework::getMemoryTypeName<type>(), old_object_count, old_bytes, new_object_count, new_bytes)

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

	// Captures RSS before a measured call; checkRSSEnd logs its change.
	void checkRSSBegin(const char* name);
	void checkRSSEnd();

	namespace internal
	{
		class RSSDiffScope
		{
		public:
			explicit RSSDiffScope(const char* name) { checkRSSBegin(name); }
			RSSDiffScope(const RSSDiffScope&) = delete;
			RSSDiffScope& operator=(const RSSDiffScope&) = delete;
			~RSSDiffScope() { checkRSSEnd(); }
		};
	}

	#define launchCheckRSSDiff(...) \
		do \
		{ \
			::danikk_framework::internal::RSSDiffScope rss_diff_scope(#__VA_ARGS__); \
			__VA_ARGS__; \
		} while(false)

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
