#pragma once

//#include <cstring>
//#include <type_traits>
#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/array.h>

namespace danikk_framework
{
	typedef size_t unicode_char;
	typedef char* CString;

	template<class T> struct is_cstring
	{
		template<class arr_t> struct ischrarr
		{
			typedef typename std::remove_pointer<arr_t>::type unpointed;
			typedef typename std::remove_reference<unpointed>::type element_type;
			static constexpr bool value = std::is_array<T>::value && std::is_same<element_type, char>::value;
		};

		static constexpr bool value =
			std::is_convertible<T, const char*>::value		||
			std::is_convertible<T, const char* const>::value||
			std::is_same<T, char*>::value 					||
			std::is_same<T, const char*>::value 			||
			std::is_same<T, char* const>::value 			||
			std::is_same<T, const char* const>::value 		||
			ischrarr<T>::value;
	};

	//Те функции, которые являются шаблонными должны были
	//возвращать char* и const char*
	//И что-бы не дублировать код, можно просто
	//сделать тип си строки шаблоном.

#define CSTRING_TEMPLATE template<class cstringT> inline cstringT

	inline bool isNumberChar(char chr)
	{
		return chr >= '0' && chr <= '9';
	}

	template<class stream_t> stream_t& operator << (stream_t& stream, const char* str)
	{
		char curchr;
		while((curchr = *str) != '\0')
		{
			stream << curchr;
			str++;
		}

		return stream;
	}

	inline bool isslash(size_t chr)
	{
		return chr == '/' || chr == '\\';
	}

	template<size_t n> inline const char* tocstring(const char value[n])
	{
		return value;
	}

	template<class cstringT> inline const char* tocstring(const cstringT& value)
	{
		if constexpr(is_cstring<cstringT>::value)
		{
			return value;
		}
		else if constexpr (std::is_same<cstringT, std::string>::value)
		{
			return value.c_str();
		}
		else
		{
			return value.c_string();
		}
	}

	bool strempty(const char* str);

	//Проверка строк на одинаковость.
	bool strequal(const char* str1, const char* str2);

	//Проверка строк на различие.
	bool strnotequal(const char* str1, const char* str2);

	//str содетжит символ chr перед первым символом end.
	bool strcontainsbefore(const char* str, char chr, char end);

	//Если str1 начинается с str2, то возвращает true.
	bool strstartswith(const char* str1, const char* str2);

	//str move if starts with
	bool strmifsw(const char*& str1, const char* str2);

	//Если str1 начинается с str2, то возвращает true.
	template<class str2T> bool strstartswith(const char* str1, const str2T& str2)
	{
		return strncmp(str1, str2.data(), str2.size());
	}
	//Если str1 начинается с str2, то возвращает true.
	template<class str2T> bool strstartswith(const char* str1,const char* str2)
	{
		return strncmp(str1, str2, strlen(str2));
	}

	//Строка состоит только из заданных символов
	bool strcontainsonly(const char* str, char chr1, char chr2, char chr3);

	//Получает последний символ из строки.
	unicode_char strlastchar(const char* str);

	//Вырезает заданный символ из строки.
	//Возвращает длинну новой строки.
	size_t strcut(char* string, char character);

	template<size_t dest_buffer_size, size_t splitter_chars_count> void
		strsplit(char* string, Array<char*, dest_buffer_size>& dest_buffer, Array<char, splitter_chars_count>& splitters)
	{
		dest_buffer.push(string);

	    while (*string != '\0')
	    {
	        for(char chr : splitters)
	        {
	        	if (*string == chr)
				{
		            *string = '\0';
		    		dest_buffer.push(string + 1);
					break;
				}
	        }

	        string++;
	    }
	}

	template<class num_t> num_t packString(const char* string)
	{
		if constexpr(sizeof(num_t) == 2 || sizeof(num_t) == 1)
		{
			return *(num_t*)string;
		}
		else
		{
			char result[sizeof(num_t)];
			memcpy(result.data(), string, std::min(strlen(string), sizeof(num_t)));
			return *(num_t*)result;
		}
	}

#undef CSTRING_TEMPLATE
}
