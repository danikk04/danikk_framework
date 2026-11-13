#include <cstdio>
#include <limits>

#if defined(_WIN32)
#define PSAPI_VERSION 2
#include <windows.h>
#include <psapi.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

#include <danikk_framework/memory.h>

namespace danikk_framework
{
	size_t getProcessMemoryUsageBytes()
	{
#if defined(_WIN32)
		PROCESS_MEMORY_COUNTERS counters{};
		if(!GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof(counters)))
		{
			return 0;
		}
		return static_cast<size_t>(counters.WorkingSetSize);
#elif defined(__APPLE__)
		mach_task_basic_info_data_t info{};
		mach_msg_type_number_t info_count = MACH_TASK_BASIC_INFO_COUNT;
		const kern_return_t result = task_info(
			mach_task_self(),
			MACH_TASK_BASIC_INFO,
			reinterpret_cast<task_info_t>(&info),
			&info_count);
		if(result != KERN_SUCCESS || info.resident_size > std::numeric_limits<size_t>::max())
		{
			return 0;
		}
		return static_cast<size_t>(info.resident_size);
#elif defined(__linux__)
		FILE* statm_file = std::fopen("/proc/self/statm", "r");
		if(statm_file == nullptr)
		{
			return 0;
		}

		unsigned long long virtual_pages = 0;
		unsigned long long resident_pages = 0;
		const int fields_read = std::fscanf(statm_file, "%llu %llu", &virtual_pages, &resident_pages);
		std::fclose(statm_file);
		if(fields_read != 2)
		{
			return 0;
		}

		const long page_size = sysconf(_SC_PAGESIZE);
		if(page_size <= 0
			|| resident_pages > std::numeric_limits<size_t>::max() / static_cast<size_t>(page_size))
		{
			return 0;
		}
		return static_cast<size_t>(resident_pages) * static_cast<size_t>(page_size);
#else
		return 0;
#endif
	}
}
