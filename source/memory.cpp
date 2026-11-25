#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <map>
#include <mutex>
#include <utility>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#else
#error "Memory type-name demangling requires the GNU C++ ABI header <cxxabi.h>."
#endif
#else
#error "Memory type-name demangling is supported only with GCC or Clang."
#endif

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
#include <danikk_framework/log.h>

namespace danikk_framework
{
	namespace
	{
		const char* rss_checkpoint_name = nullptr;
		size_t rss_checkpoint_bytes = 0;
		bool rss_checkpoint_active = false;
		bool rss_checkpoint_open = false;
		struct MemoryStatsState
		{
			struct Entry
			{
				std::string raw_type_name;
				MemoryTypeStats stats;
			};

			std::mutex mutex;
			std::map<MemoryTypeId, Entry> entries;
		};

		MemoryStatsState& getMemoryStatsState()
		{
			static MemoryStatsState state;
			return state;
		}

		struct MemoryStatsIndexDescending
		{
			const std::vector<MemoryTypeStats>& stats;

			bool operator()(size_t left_index, size_t right_index) const
			{
				const MemoryTypeStats& left = stats[left_index];
				const MemoryTypeStats& right = stats[right_index];
				if(left.reserved_bytes != right.reserved_bytes)
				{
					return left.reserved_bytes > right.reserved_bytes;
				}
				return left.type_name < right.type_name;
			}
		};

		struct MemorySizeDisplay
		{
			uint64 whole;
			uint64 fraction;
			const char* suffix;
			bool has_fraction;
		};

		MemorySizeDisplay getMemorySizeDisplay(size_t bytes)
		{
			if(bytes >= mib(1))
			{
				const uint64 divisor = mib(1);
				const uint64 remainder = bytes % divisor;
				return {bytes / divisor, (remainder * 100 + divisor / 2) / divisor, "MiB", true};
			}
			if(bytes >= kib(1))
			{
				const uint64 divisor = kib(1);
				const uint64 remainder = bytes % divisor;
				return {bytes / divisor, (remainder * 100 + divisor / 2) / divisor, "KiB", true};
			}
			return {bytes, 0, "B", false};
		}

		void writeMemorySizeDisplay(char* buffer, size_t buffer_size, const MemorySizeDisplay& display)
		{
			if(display.has_fraction)
			{
				uint64 whole = display.whole;
				uint64 fraction = display.fraction;
				if(fraction == 100)
				{
					++whole;
					fraction = 0;
				}
				std::snprintf(buffer, buffer_size, "%llu.%02llu%s", static_cast<unsigned long long>(whole), static_cast<unsigned long long>(fraction), display.suffix);
				return;
			}
			std::snprintf(buffer, buffer_size, "%llu%s", static_cast<unsigned long long>(display.whole), display.suffix);
		}

		std::string getDisplayTypeName(const std::string& raw_type_name)
		{
			int status = 0;
			char* demangled_name = abi::__cxa_demangle(raw_type_name.c_str(), nullptr, nullptr, &status);
			if(status != 0 || demangled_name == nullptr)
			{
				std::free(demangled_name);
				return raw_type_name;
			}
			std::string display_name = demangled_name;
			std::free(demangled_name);
			return display_name;
		}

		MemoryTypeStats& getMemoryTypeStats(MemoryStatsState& state, MemoryTypeId type_id, const char* type_name)
		{
			MemoryTypeId resolved_type_id = type_id;
			const std::string resolved_type_name = type_name == nullptr ? "unknown" : type_name;
			while(true)
			{
				auto stats_iterator = state.entries.find(resolved_type_id);
				if(stats_iterator == state.entries.end())
				{
					MemoryStatsState::Entry entry;
					entry.raw_type_name = resolved_type_name;
					entry.stats.type_id = resolved_type_id;
					entry.stats.type_name = getDisplayTypeName(resolved_type_name);
					return state.entries.emplace(resolved_type_id, std::move(entry)).first->second.stats;
				}
				if(stats_iterator->second.raw_type_name == resolved_type_name)
				{
					return stats_iterator->second.stats;
				}
				++resolved_type_id;
			}
		}
	}

	void registerMemoryAlloc(MemoryTypeId type_id, const char* type_name, size_t object_count, size_t bytes)
	{
		if(bytes == 0)
		{
			return;
		}
		MemoryStatsState& state = getMemoryStatsState();
		std::lock_guard<std::mutex> lock(state.mutex);
		MemoryTypeStats& stats = getMemoryTypeStats(state, type_id, type_name);
		++stats.active_allocations;
		stats.reserved_objects += object_count;
		stats.reserved_bytes += bytes;
		stats.peak_reserved_bytes = std::max(stats.peak_reserved_bytes, stats.reserved_bytes);
		stats.total_allocated_bytes += bytes;
	}

	void registerMemoryDealloc(MemoryTypeId type_id, const char* type_name, size_t object_count, size_t bytes)
	{
		if(bytes == 0)
		{
			return;
		}
		MemoryStatsState& state = getMemoryStatsState();
		std::lock_guard<std::mutex> lock(state.mutex);
		MemoryTypeStats& stats = getMemoryTypeStats(state, type_id, type_name);
		if(stats.active_allocations > 0)
		{
			--stats.active_allocations;
		}
		stats.reserved_objects = stats.reserved_objects >= object_count ? stats.reserved_objects - object_count : 0;
		stats.reserved_bytes = stats.reserved_bytes >= bytes ? stats.reserved_bytes - bytes : 0;
	}

	void registerMemoryRealloc(MemoryTypeId type_id, const char* type_name, size_t old_object_count, size_t old_bytes, size_t new_object_count, size_t new_bytes)
	{
		if(old_bytes == 0 && new_bytes == 0)
		{
			return;
		}
		MemoryStatsState& state = getMemoryStatsState();
		std::lock_guard<std::mutex> lock(state.mutex);
		MemoryTypeStats& stats = getMemoryTypeStats(state, type_id, type_name);
		stats.reserved_objects = stats.reserved_objects >= old_object_count ? stats.reserved_objects - old_object_count : 0;
		stats.reserved_bytes = stats.reserved_bytes >= old_bytes ? stats.reserved_bytes - old_bytes : 0;
		stats.reserved_objects += new_object_count;
		stats.reserved_bytes += new_bytes;
		if(old_bytes == 0 && new_bytes > 0)
		{
			++stats.active_allocations;
		}
		else if(old_bytes > 0 && new_bytes == 0 && stats.active_allocations > 0)
		{
			--stats.active_allocations;
		}
		stats.total_allocated_bytes += new_bytes;
		stats.peak_reserved_bytes = std::max(stats.peak_reserved_bytes, stats.reserved_bytes);
	}

	void logMemoryUse(uint32 max_entry)
	{
		std::vector<MemoryTypeStats> sorted_stats;
		std::vector<size_t> sorted_indices;
		{
			MemoryStatsState& state = getMemoryStatsState();
			std::lock_guard<std::mutex> lock(state.mutex);
			for(const auto& entry : state.entries)
			{
				if(entry.second.stats.reserved_bytes > 0)
				{
					sorted_indices.push_back(sorted_stats.size());
					sorted_stats.push_back(entry.second.stats);
				}
			}
		}
		std::sort(sorted_indices.begin(), sorted_indices.end(), MemoryStatsIndexDescending{sorted_stats});
		const size_t entry_count = std::min(static_cast<size_t>(max_entry), sorted_indices.size());
		for(size_t index = 0; index < entry_count; ++index)
		{
			const MemoryTypeStats& stats = sorted_stats[sorted_indices[index]];
			char reserved_text[64];
			char peak_text[64];
			writeMemorySizeDisplay(reserved_text, sizeof(reserved_text), getMemorySizeDisplay(stats.reserved_bytes));
			writeMemorySizeDisplay(peak_text, sizeof(peak_text), getMemorySizeDisplay(stats.peak_reserved_bytes));
			formatLogMemory("Memory use: type=% reserved=% active_allocations=% peak=%", stats.type_name, reserved_text, stats.active_allocations, peak_text);
		}
	}

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

	void checkRSSBegin(const char* name)
	{
		if(rss_checkpoint_open)
		{
			formatLogWarning("RSS checkpoint '%': ended implicitly by a new checkpoint", rss_checkpoint_name);
			checkRSSEnd();
		}

		rss_checkpoint_name = name == nullptr ? "unnamed" : name;
		rss_checkpoint_bytes = getProcessMemoryUsageBytes();
		rss_checkpoint_open = true;
		rss_checkpoint_active = rss_checkpoint_bytes != 0;
		if(!rss_checkpoint_active)
		{
			formatLogWarning("RSS checkpoint '%': unable to read starting process memory usage", rss_checkpoint_name);
		}
	}

	void checkRSSEnd()
	{
		if(!rss_checkpoint_open)
		{
			formatLogWarning("RSS checkpoint end called without begin");
			return;
		}
		if(!rss_checkpoint_active)
		{
			rss_checkpoint_open = false;
			rss_checkpoint_name = nullptr;
			return;
		}

		const size_t current_rss_bytes = getProcessMemoryUsageBytes();
		if(current_rss_bytes == 0)
		{
			formatLogWarning("RSS checkpoint '%': unable to read ending process memory usage", rss_checkpoint_name);
		}
		else if(current_rss_bytes >= rss_checkpoint_bytes)
		{
			formatLogMemory(
				"RSS checkpoint '%': +% bytes (+% MiB), RSS: % bytes (% MiB)",
				rss_checkpoint_name,
				current_rss_bytes - rss_checkpoint_bytes,
				(current_rss_bytes - rss_checkpoint_bytes) / mib(1),
				current_rss_bytes,
				current_rss_bytes / mib(1));
		}
		else
		{
			formatLogMemory(
				"RSS checkpoint '%': -% bytes (-% MiB), RSS: % bytes (% MiB)",
				rss_checkpoint_name,
				rss_checkpoint_bytes - current_rss_bytes,
				(rss_checkpoint_bytes - current_rss_bytes) / mib(1),
				current_rss_bytes,
				current_rss_bytes / mib(1));
		}
		rss_checkpoint_active = false;
		rss_checkpoint_open = false;
		rss_checkpoint_name = nullptr;
	}
}
