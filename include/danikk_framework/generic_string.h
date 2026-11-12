#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/assert.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string_view>
#include <type_traits>

namespace danikk_framework
{
	template<class derived_t> class GenericString
	{
	private:
		derived_t& derived() { return static_cast<derived_t&>(*this); }
		const derived_t& derived() const { return static_cast<const derived_t&>(*this); }
	public:
		char* c_string() { return derived().data(); }
		char first() const { return derived().empty() ? '\0' : derived()[0]; }
		char last() const { return derived().empty() ? '\0' : derived()[derived().size() - 1]; }

		derived_t& operator<<(char value)
		{
			appendData(&value, 1);
			return derived();
		}

		derived_t& operator<<(const char* value)
		{
			appendData(value, std::strlen(value));
			return derived();
		}

		template<class other_t>
		requires requires(const other_t& value) { value.data(); value.size(); }
		derived_t& operator<<(const other_t& value)
		{
			appendData(value.data(), value.size());
			return derived();
		}

		derived_t& operator<<(float value)
		{
			char buffer[64];
			std::snprintf(buffer, sizeof(buffer), "%.2f", static_cast<double>(value));
			appendData(buffer, std::strlen(buffer));
			return derived();
		}

		derived_t& operator<<(uint64 value)
		{
			char buffer[64];
			std::snprintf(buffer, sizeof(buffer), "%llu", static_cast<unsigned long long>(value));
			appendData(buffer, std::strlen(buffer));
			return derived();
		}

		derived_t& copyData(const char* source, size_t count)
		{
			derived().resize(count);
			if(count != 0) std::memmove(derived().data(), source, count);
			return derived();
		}
		derived_t& appendData(const char* source, size_t count)
		{
			const size_t old_size = derived().size();
			if(count > std::numeric_limits<size_t>::max() - old_size) std::abort();
			const uintptr_t base = reinterpret_cast<uintptr_t>(derived().data());
			const uintptr_t input = reinterpret_cast<uintptr_t>(source);
			const bool aliases = input >= base && input - base <= old_size;
			const size_t offset = aliases ? input - base : 0;
			derived().resize(old_size + count);
			if(aliases) source = derived().data() + offset;
			if(count != 0) std::memmove(derived().data() + old_size, source, count);
			return derived();
		}
		derived_t& copyData(const char* source) { return copyData(source, std::strlen(source)); }
		derived_t& copyData(const char* first, const char* last) { return copyData(first, static_cast<size_t>(last - first)); }
		template<class other_t> derived_t& copyData(const other_t& source) { return copyData(source.data(), source.size()); }

		template<class other_t>
		requires requires(const other_t& value) { value.data(); value.size(); }
		bool equals(const other_t& other) const
		{
			return derived().size() == other.size()
				&& (derived().empty() || std::memcmp(derived().data(), other.data(), derived().size()) == 0);
		}
		bool equals(const char* other) const { return std::string_view(derived().data(), derived().size()) == std::string_view(other); }
		template<class other_t>
		requires requires(const other_t& value) { value.data(); value.size(); }
		bool isLessThan(const other_t& other) const
		{
			const size_t common = derived().size() < other.size() ? derived().size() : other.size();
			const int order = common == 0 ? 0 : std::memcmp(derived().data(), other.data(), common);
			return order < 0 || (order == 0 && derived().size() < other.size());
		}

		bool contains(char value) const { return findIndex(value) != derived().size(); }
		bool startsWith(const char* prefix) const
		{
			const size_t prefix_size = std::strlen(prefix);
			return prefix_size <= derived().size() && std::memcmp(derived().data(), prefix, prefix_size) == 0;
		}

		size_t findIndex(char value) const { for(size_t i = 0; i < derived().size(); ++i) if(derived()[i] == value) return i; return derived().size(); }
		size_t findLastIndex(char value) const { for(size_t i = derived().size(); i > 0; --i) if(derived()[i - 1] == value) return i - 1; return derived().size(); }
		char* findPtr(char value) { size_t i = findIndex(value); return i == derived().size() ? nullptr : derived().data() + i; }
		const char* findPtr(char value) const { size_t i = findIndex(value); return i == derived().size() ? nullptr : derived().data() + i; }
		char* findLast(char value) { size_t i = findLastIndex(value); return i == derived().size() ? nullptr : derived().data() + i; }
		const char* findLast(char value) const { size_t i = findLastIndex(value); return i == derived().size() ? nullptr : derived().data() + i; }

		derived_t& substring(const char* first, const char* last)
		{
			const size_t count = static_cast<size_t>(last - first);
			copyData(first, count);
			return derived();
		}
		derived_t& cutAfterSeparator(char separator) { size_t p = findIndex(separator); if(p != derived().size()) copyData(derived().data() + p + 1, derived().size() - p - 1); return derived(); }
		derived_t& cutBeforeSeparator(char separator) { size_t p = findIndex(separator); if(p != derived().size()) derived().resize(p); return derived(); }
		derived_t& cutAfterLastSeparator(char separator) { size_t p = findLastIndex(separator); if(p != derived().size()) copyData(derived().data() + p + 1, derived().size() - p - 1); return derived(); }
		derived_t& cutBeforeLastSeparator(char separator) { size_t p = findLastIndex(separator); if(p != derived().size()) derived().resize(p); return derived(); }
		derived_t& cutSideChars(char value)
		{
			size_t first = 0;
			while(first < derived().size() && derived()[first] == value) ++first;
			size_t last = derived().size();
			while(last > first && derived()[last - 1] == value) --last;
			if(first != 0 && last > first) std::memmove(derived().data(), derived().data() + first, last - first);
			derived().resize(last - first);
			return derived();
		}
		derived_t& cutChars(char value)
		{
			size_t write = 0;
			for(size_t read = 0; read < derived().size(); ++read)
				if(derived()[read] != value) derived()[write++] = derived()[read];
			derived().resize(write);
			return derived();
		}

		template<class other_t> derived_t& eraseCharacters(const other_t& targets)
		{
			size_t write = 0;
			for(size_t read = 0; read < derived().size(); ++read)
			{
				bool remove = false;
				for(char target : targets) if(derived()[read] == target) { remove = true; break; }
				if(!remove) derived()[write++] = derived()[read];
			}
			derived().resize(write);
			return derived();
		}

		template<class sub_t, class replacement_t>
		derived_t& replace(const sub_t& sub, const replacement_t& replacement)
		{
			if(sub.size() == 0) return derived();
			derived_t result;
			for(size_t position = 0; position < derived().size();)
			{
				if(position + sub.size() <= derived().size()
					&& std::memcmp(derived().data() + position, sub.data(), sub.size()) == 0)
				{
					result.appendData(replacement.data(), replacement.size());
					position += sub.size();
				}
				else result.appendData(derived().data() + position++, 1);
			}
			derived().resize(result.size());
			if(result.size() != 0) std::memcpy(derived().data(), result.data(), result.size());
			return derived();
		}

		template<class other_t> derived_t& appendToStart(const other_t& prefix)
		{
			derived_t result;
			result.appendData(prefix.data(), prefix.size());
			result.appendData(derived().data(), derived().size());
			derived().resize(result.size());
			if(result.size() != 0) std::memcpy(derived().data(), result.data(), result.size());
			return derived();
		}

		template<class other_t, class output1_t, class output2_t>
		void split(const other_t& separator_position, output1_t& first, output2_t& second) const
		{
			const char* position = separator_position;
			const char* begin = derived().data();
			const char* end = begin + derived().size();
			assert(position >= begin && position < end);
			first.resize(static_cast<size_t>(position - begin));
			if(first.size()) std::memcpy(first.data(), begin, first.size());
			second.resize(static_cast<size_t>(end - position - 1));
			if(second.size()) std::memcpy(second.data(), position + 1, second.size());
		}

		template<class output1_t, class output2_t> void splitToFirstSeparator(char separator, output1_t& first, output2_t& second) const
		{
			auto p = findIndex(separator); if(p == derived().size()) { first.clear(); second.clear(); return; }
			first.resize(p); if(p) std::memcpy(first.data(), derived().data(), p); second.resize(derived().size() - p - 1); if(second.size()) std::memcpy(second.data(), derived().data() + p + 1, second.size());
		}
		template<class output1_t, class output2_t> void splitToLastSeparator(char separator, output1_t& first, output2_t& second) const
		{
			auto p = findLastIndex(separator); if(p == derived().size()) { first.clear(); second.clear(); return; }
			first.resize(p); if(p) std::memcpy(first.data(), derived().data(), p); second.resize(derived().size() - p - 1); if(second.size()) std::memcpy(second.data(), derived().data() + p + 1, second.size());
		}
	};
}
