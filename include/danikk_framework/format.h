#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/cstring_functions.h>
#include <danikk_framework/string.h>
#include <danikk_framework/number.h>

namespace danikk_framework
{
	template<class out_t> void formatAppend(out_t& out, const char* format_str)
	{
		while(*format_str)
		{
			char current_char = *format_str;
			switch(current_char)
			{
			case '\0':
				return;
			case '%':
				return;
			default:
				out << current_char;
				break;
			}
			format_str++;
		}
	}

	template<class out_t, class first_t> void formatAppend(out_t& out, const char* format_str, const first_t& first)
	{
		while(*format_str)
		{
			char current_char = *format_str;
			switch(current_char)
			{
			case '\0':
				return;
			case '\\':
				if(*(format_str + 1) == '%')
				{
					format_str++;
					out << (char)'%';
				}
				else
				{
					out << (char)'\\';
				}
				break;
			case '%':
				if constexpr (is_number<first_t>::value && !std::is_same<first_t, char>::value)
				{
					char buffer[64];
					writeNumberToStringBuffer(first, buffer, 64);
					out << buffer;
				}
				else
				{
					out << first;
				}
				formatAppend(out, format_str + 1);
				return;
			default:
				out << current_char;
				break;
			}
			format_str++;
		}
	}

	template<class out_t, class first_t, class ...other_t> void formatAppend(out_t& out, const char* format_str, const first_t& first, const other_t&... args)
	{
		while(*format_str)
		{
			char current_char = *format_str;

			switch(current_char)
			{
			case '\0':
				return;
			case '\\':
				if(*(format_str + 1) == '%')
				{
					format_str++;
					out << (char)'%';
				}
				else
				{
					out << (char)'\\';
				}
				break;
			case '%':
				if constexpr (is_number<first_t>::value && !std::is_same<first_t, char>::value)
				{
					char buffer[64];
					writeNumberToStringBuffer(first, buffer, 64);
					out << buffer;
				}
				else
				{
					out << first;
				}
				formatAppend(out, format_str+1, args...);
				return;
			default:
				out << current_char;
				break;
			}
			format_str++;
		}
	}

	template<class out_t, class ...args_t> void format(out_t& out, const char* format_str, const args_t&... args)
	{
		out.clear();
		formatAppend(out, format_str, args...);
	}

	template<class out_t, class ...args_t> void formatWrite(out_t& out, const char* format_str, const args_t&... args)
	{
		formatAppend(out, format_str, args...);
	}

	template<class out_t, class value_t> void formatAppendList(out_t& out, const char* split_str , value_t* iter, value_t* end, auto get_str)
	{
		while(true)
		{
			formatAppend(out, " % ", get_str(*iter));
			iter++;
			if(iter != end)
			{
				out << split_str;
			}
			else
			{
				return;
			}
		}
	}

	template<class out_t, class value_t> void formatAppendList(out_t& out, const char* split_str , value_t* iter, value_t* end)
	{
		while(true)
		{
			formatAppend(out, " % ", *iter);
			iter++;
			if(iter != end)
			{
				out << split_str;
			}
			else
			{
				return;
			}
		}
	}

	template<class ...args_t> String formatCreate(const char* format, const args_t&... args)
	{
		String result;
		formatAppend(result, format, args...);
		return move(result);
	}



	typedef decltype(std::endl<char, std::char_traits<char>>) endl_t;

    template<class stream_t> stream_t& operator << (stream_t& stream, endl_t value)
    {
    	(void)value;
    	stream << '\n';
    	return stream;
    }

    using std::ostream;

	struct indent
	{
		size_t size;
	};

	template<class stream_t> stream_t& operator << (stream_t& cout, indent indent)
	{
		while(indent.size-- > 0)
		{
			cout << ' ';
		}
		return cout;
	}

	template<class string_t, char left_char, char right_char = left_char> struct side_chars
	{
		string_t* str;

		side_chars(string_t& str)
		{
			this->str = &str;
		}
	};

	template<class stringstream_t, class string_t, char left_char, char right_char> stringstream_t& operator
		<<(stringstream_t& cout, side_chars<string_t, left_char, right_char> string)
	{
		cout << left_char << *string.str << right_char;
		return cout;
	}

	template<class string_t> struct single_quotes : side_chars<string_t, '\''>
	{
		single_quotes(string_t& str) : side_chars<string_t, '\''>(str){}
	};

	template<class string_t> struct double_quotes : side_chars<string_t, '\"'>
	{
		double_quotes(string_t& str) : side_chars<string_t, '\"'>(str){}
	};

	template<class string_t> struct brackets : side_chars<string_t, '(', ')'>
	{
		brackets(string_t& str) : side_chars<string_t, '(', ')'>(str){}
	};

	template<class string_t> struct square_brackets : side_chars<string_t, '[', ']'>
	{
		square_brackets(string_t& str) : side_chars<string_t, '[', ']'>(str){}
	};

	template<class number_t> struct zero_aligned
	{
		number_t value;
		size_t number_count;
	};

	template<class stream_t, class number_t> stream_t& operator << (stream_t& out, const zero_aligned<number_t>& data)
	{
		size_t number_count = 1;

		for(number_t i = data.value; (i /= 10) >= 1; number_count++);//Вычисление количества цифр в числе.

		if(number_count <= data.number_count)
		{
			size_t zero_count = data.number_count - number_count;

			while(zero_count --> 0)
			{
				out << '0';
			}
		}
		out << (uint64)data.value;

		return out;
	}

	template<char chr,class str_t> struct aligned
	{
		str_t str;
		size_t target_char_count;
	};

	template<class stream_t, char chr, class str_t> stream_t& operator << (stream_t& out, const aligned<chr,str_t>& data)
	{
		size_t char_count = data.target_char_count - data.str;
		while(char_count --> 0)
		{
			out << chr;
		}
		out << data.str;
		return out;
	}

	struct n_chars
	{
		char c;
		byte n;

		n_chars(char c, byte n) : c(c), n(n){}
	};

	template<class stream_t> stream_t& operator << (stream_t& out, const n_chars& data)
	{
		int counter = data.n;

		while(counter --> 0)
		{
			out << data.c;
		}
		return out;
	}

	struct truncated_display
	{
		float value;
		uint32 count;

		truncated_display(float value, uint32 count) : value(value), count(count){}
	};

	template<class stream_t> stream_t& operator << (stream_t& out, const truncated_display& data)
	{
		char buffer[64];
		writeNumberToStringBuffer(data.value, buffer, 64);
		char* current_char = buffer;

		while(*current_char != '.')
		{
			out << *current_char;
			current_char++;
		}

		current_char++;
		uint count = data.count;

		while(*current_char != '\0' && count > 0)
		{
			out << (char)*current_char;
			current_char++;
			count--;
		}

		return out;
	}

	/*template<class array_t> struct array_string_display
	{
		array_t& array;
		char separator;
	};


	template<class stream_t, class array_t> stream_t& operator << (stream_t& out, const array_string_display<array_t>& data)
	{
		for(index_t i = 0; i < data.array.size(); i++)
		{
			const typename array_t::value_type& element = data.array[i];

			out << element;

			if(i < data.array.size() - 1)
			{
				out << data.separator;
			}
		}
		return out;
	}*/
}
