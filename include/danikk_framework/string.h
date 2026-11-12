#pragma once

#include <danikk_framework/generic_string.h>
#include <string>

namespace danikk_framework
{
	class String : public std::string, public GenericString<String>
	{
	public:
		using std::string::operator=;

		String() = default;
		String(const String&) = default;
		String(String&&) noexcept = default;
		String& operator=(const String&) = default;
		String& operator=(String&&) noexcept = default;
		String(const std::string& value) : std::string(value) {}
		String(std::string&& value) noexcept : std::string(danikk_framework::move(value)) {}
		String(const char* value) : std::string(value == nullptr ? "" : value) {}
		String(const char* first, const char* last) : std::string(first, static_cast<size_t>(last - first)) {}
		String(const char* value, size_t size) : std::string(value == nullptr ? "" : value, value == nullptr ? 0 : size) {}
		String(char* value) : String(static_cast<const char*>(value)) {}
		String(char* value, size_t size) : String(static_cast<const char*>(value), size) {}
		String(char* value, size_t size, size_t) : String(static_cast<const char*>(value), size) {}
		String(size_t size) : std::string(size, '\0') {}
		String(size_t size, char fill) : std::string(size, fill) {}

		const char* c_string() const { return data(); }
		operator bool() const { return !empty(); }
		void substractsize(size_t count)
		{
			assert(count <= size());
			resize(size() - count);
		}
	};
}
