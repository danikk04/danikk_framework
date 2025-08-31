#pragma once

#include <danikk_framework/number_base.h>
#include <danikk_framework/string_buffer.h>

namespace danikk_framework
{
	template<class numberT> size_t writeNumberToStringBuffer(numberT number, char* buffer, size_t bufferSize)
	{
		size_t len;

#define printToBuffer(format, type) snprintf(buffer, bufferSize, format, (type)number)
		if constexpr(std::is_signed<numberT>::value && std::is_integral<numberT>::value)
		{
			len = printToBuffer("%lli", long long int);
		}
		else if constexpr(std::is_unsigned<numberT>::value && std::is_integral<numberT>::value)
		{
			len = printToBuffer("%llu", long long unsigned int);
		}
		else if constexpr(std::is_same<numberT, float>::value)
		{
			len = printToBuffer("%.2f", float);
		}
		else if constexpr(std::is_same<numberT, double>::value)
		{
			len = printToBuffer("%lf", double);
		}
		else
		{
			static_assert(true, "this is not nubmer");//Это не число.
		}
#undef snprintf
		return len;
	}

	template<class numberT> String numberToString(numberT number)
	{
		char buffer[64];
		size_t len = writeNumberToStringBuffer(number, buffer, sizeof(buffer));
		return String((const char*)buffer, len);
	}

	template<class numberT> numberT parseNumber(const char* data)
	{
		if (data == NULL)
		{
			return 0;
		}
		if constexpr ((std::is_integral<numberT>::value && sizeof(numberT) < 4) || std::same_as<numberT, int>)
		{
			return (numberT)std::atoi(data);
		}
		else if constexpr(std::is_integral<numberT>::value)
		{
			char* a;
			return (numberT)std::strtoll(data,&a,10);
		}
		else if constexpr (std::is_floating_point<numberT>::value)
		{
			return (numberT)atof(data);
		}
		else
		{
			static_assert(true, "parseNumber::data != num");
		}
	}

	template<class numberT> numberT parseNumber(const String& data)
	{
		return parseNumber<numberT>(data.c_string());
	}

	template<class numberT> numberT readNumber(const char* str)
	{
		StringBuffer<64> numbuffer;
		while(true)
		{
			char chr = *str;
			if(isNumberChar(chr))
			{
				assert(numbuffer.size() < 63);
				numbuffer << chr;
			}
			else
			{
				if(chr != '\0')
				{
					str++;
				}
				return parseNumber<numberT>(numbuffer.c_string());
			}
			str++;
		}
	}

	//float_t может являться double
	template<class float_t> struct truncatedFloat
	{
		float_t value;
	};

	template<class stream_t, class float_t> stream_t operator<<(stream_t& out, truncatedFloat<float_t> tf)
	{
		char buffer[64];
		writeNumberToStringBuffer(tf.value, buffer, 64);
		char* current_char = buffer;

		while(*current_char != '.')
		{
			out << *current_char;
			current_char++;
		}

		if(*(current_char + 1) == '0')//Для игнорирования точки, а то раньше вместо "15" могло написать "15."
		{
			return out;
		}

		while(*current_char != '\0' && *current_char != '0')
		{
			out << (char)*current_char;
			current_char++;
		}

		return out;
	}

	/*template <class streamT, class num_t> std::enable_if
	<
		is_number<num_t>::value &&
		!std::is_same<num_t, char>::value, streamT&
	>::type operator << (streamT& cout, num_t num)
	{
		if constexpr (std::is_same<num_t, char>::value)
		{
			cout << (char)num;
		}
		else
		{
			char buffer[64];
			writeNumberToStringBuffer(num, buffer, 64);
			cout << (const char*)buffer;
			return cout;
		}
		return cout;
	}*/
}
