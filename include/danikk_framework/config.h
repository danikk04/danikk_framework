#pragma once

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <type_traits>

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/dictionary.h>
#include <danikk_framework/dynamic_array.h>
#include <danikk_framework/string.h>

namespace danikk_framework
{
	using ConfigValueParser = bool (*)(void* destination, const char* value, size_t value_size);

	enum class ConfigLoadError : uint8
	{
		none = 0,
		invalid_input,
		malformed_line,
		unknown_key,
		invalid_value
	};

	class Config
	{
		struct FieldBinding
		{
			String key;
			void* destination = nullptr;
			ConfigValueParser parser = nullptr;

			FieldBinding(const char* key, void* destination, ConfigValueParser parser)
				: key(key), destination(destination), parser(parser) {}
			FieldBinding(const FieldBinding&) = delete;
			FieldBinding& operator=(const FieldBinding&) = delete;
			FieldBinding(FieldBinding&&) noexcept = default;
			FieldBinding& operator=(FieldBinding&&) noexcept = default;
		};

		DynamicArray<FieldBinding> fields;

		static bool isWhitespace(char value)
		{
			return value == ' ' || value == '\t' || value == '\r' || value == '\f' || value == '\v';
		}

		static bool equalKey(const String& registered_key, const char* key, size_t key_size)
		{
			return registered_key.size() == key_size
				&& (key_size == 0 || std::memcmp(registered_key.data(), key, key_size) == 0);
		}

		template<class value_t>
		static bool parseBoundValue(void* destination, const char* value, size_t value_size)
		{
			using stored_t = std::remove_cv_t<value_t>;
			stored_t& output = *static_cast<stored_t*>(destination);
			if constexpr(std::is_same_v<stored_t, String>)
			{
				output = String(value, value_size);
				return true;
			}
			else if constexpr(std::is_same_v<stored_t, bool>)
			{
				if(value_size == 1 && value[0] == '1') output = true;
				else if(value_size == 1 && value[0] == '0') output = false;
				else if(value_size == 4 && std::memcmp(value, "true", 4) == 0) output = true;
				else if(value_size == 5 && std::memcmp(value, "false", 5) == 0) output = false;
				else return false;
				return true;
			}
			else if constexpr(std::is_integral_v<stored_t> && std::is_signed_v<stored_t>)
			{
				String text(value, value_size);
				char* end = nullptr;
				errno = 0;
				const long long parsed = std::strtoll(text.data(), &end, 10);
				if(errno == ERANGE || end == text.data() || end != text.data() + text.size()
					|| parsed < static_cast<long long>(std::numeric_limits<stored_t>::min())
					|| parsed > static_cast<long long>(std::numeric_limits<stored_t>::max())) return false;
				output = static_cast<stored_t>(parsed);
				return true;
			}
			else if constexpr(std::is_integral_v<stored_t> && std::is_unsigned_v<stored_t>)
			{
				if(value_size > 0 && value[0] == '-') return false;
				String text(value, value_size);
				char* end = nullptr;
				errno = 0;
				const unsigned long long parsed = std::strtoull(text.data(), &end, 10);
				if(errno == ERANGE || end == text.data() || end != text.data() + text.size()
					|| parsed > static_cast<unsigned long long>(std::numeric_limits<stored_t>::max())) return false;
				output = static_cast<stored_t>(parsed);
				return true;
			}
			else if constexpr(std::is_floating_point_v<stored_t>)
			{
				String text(value, value_size);
				char* end = nullptr;
				errno = 0;
				const long double parsed = std::strtold(text.data(), &end);
				if(errno == ERANGE || end == text.data() || end != text.data() + text.size()
					|| !std::isfinite(parsed)
					|| parsed < static_cast<long double>(std::numeric_limits<stored_t>::lowest())
					|| parsed > static_cast<long double>(std::numeric_limits<stored_t>::max())) return false;
				output = static_cast<stored_t>(parsed);
				return true;
			}
			else
			{
				static_assert(std::is_same_v<stored_t, void>,
					"Config::bind supports arithmetic fields and danikk_framework::String; use a custom parser for other types");
				return false;
			}
		}

		bool fail(ConfigLoadError error, size_t line)
		{
			last_error = error;
			last_error_line = line;
			return false;
		}

	public:
		ConfigLoadError last_error = ConfigLoadError::none;
		size_t last_error_line = 0;

		Config() = default;
		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;
		Config(Config&&) = default;
		Config& operator=(Config&&) = default;

		// The bound field must outlive this Config or remain at the same address until unbound.
		template<class value_t>
		requires (!std::is_const_v<value_t>
			&& (std::is_arithmetic_v<value_t> || std::is_same_v<value_t, String>))
		bool bind(const char* key, value_t& value)
		{
			if(key == nullptr || key[0] == '\0') return false;
			return bind(key, &value, &parseBoundValue<value_t>);
		}

		// Custom parsers can bind enums or application-specific field types.
		bool bind(const char* key, void* destination, ConfigValueParser parser)
		{
			if(key == nullptr || key[0] == '\0' || destination == nullptr || parser == nullptr) return false;
			for(FieldBinding& field : fields)
			{
				if(!equalKey(field.key, key, std::strlen(key))) continue;
				field.destination = destination;
				field.parser = parser;
				return true;
			}
			fields.pushMove(FieldBinding(key, destination, parser));
			return true;
		}

		void clearBindings()
		{
			fields.clear();
		}

		// Parses key=value lines without modifying the input buffer. Unknown keys, malformed lines,
		// and values that cannot be converted stop the load and report the first failing line.
		bool load(const char* data, size_t data_size)
		{
			last_error = ConfigLoadError::none;
			last_error_line = 0;
			if(data == nullptr && data_size != 0) return fail(ConfigLoadError::invalid_input, 0);

			size_t line_number = 1;
			for(size_t line_start = 0; line_start < data_size; line_number++)
			{
				size_t line_end = line_start;
				while(line_end < data_size && data[line_end] != '\n') line_end++;

				size_t content_start = line_start;
				size_t content_end = line_end;
				while(content_start < content_end && isWhitespace(data[content_start])) content_start++;
				while(content_end > content_start && isWhitespace(data[content_end - 1])) content_end--;
				if(content_start < content_end && data[content_start] != '#' && data[content_start] != ';')
				{
					size_t separator = content_start;
					while(separator < content_end && data[separator] != '=') separator++;
					if(separator == content_end) return fail(ConfigLoadError::malformed_line, line_number);

					size_t key_start = content_start;
					size_t key_end = separator;
					size_t value_start = separator + 1;
					size_t value_end = content_end;
					while(key_end > key_start && isWhitespace(data[key_end - 1])) key_end--;
					while(value_start < value_end && isWhitespace(data[value_start])) value_start++;
					if(key_start == key_end) return fail(ConfigLoadError::malformed_line, line_number);

					FieldBinding* matching_field = nullptr;
					for(FieldBinding& field : fields)
					{
						if(equalKey(field.key, data + key_start, key_end - key_start))
						{
							matching_field = &field;
							break;
						}
					}
					if(matching_field == nullptr) return fail(ConfigLoadError::unknown_key, line_number);
					if(!matching_field->parser(
						matching_field->destination,
						data + value_start,
						value_end - value_start))
					{
						return fail(ConfigLoadError::invalid_value, line_number);
					}
				}

				if(line_end == data_size) break;
				line_start = line_end + 1;
			}
			return true;
		}

		bool load(const char* null_terminated_data)
		{
			if(null_terminated_data == nullptr)
			{
				last_error = ConfigLoadError::invalid_input;
				last_error_line = 0;
				return false;
			}
			return load(null_terminated_data, std::strlen(null_terminated_data));
		}

		bool load(const String& data)
		{
			return load(data.data(), data.size());
		}
	};

	// Compatibility parser for offset-based dictionaries. It mutates the input buffer in place.
	template<class offset_t>
	void inlineParseConfig(DynamicInlineStringDictionary<offset_t>& out, char* data)
	{
		out.clear();
		out.setData(data);
		if(data == nullptr) return;

		const size_t data_size = std::strlen(data);
		for(size_t line_start = 0; line_start < data_size;)
		{
			size_t line_end = line_start;
			while(line_end < data_size && data[line_end] != '\n') line_end++;
			size_t content_end = line_end;
			if(content_end > line_start && data[content_end - 1] == '\r') content_end--;

			size_t separator = line_start;
			while(separator < content_end && data[separator] != '=') separator++;
			if(separator < content_end)
			{
				data[separator] = '\0';
				data[content_end] = '\0';
				out.addPair(static_cast<offset_t>(line_start), static_cast<offset_t>(separator + 1));
			}

			if(line_end == data_size) break;
			line_start = line_end + 1;
		}
	}
}
