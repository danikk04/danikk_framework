#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/string.h>
#include <danikk_framework/type_traits.h>
#include <danikk_framework/assert.h>
#include <cstdlib>
#include <concepts>
#include <type_traits>
#include "static_string.h"

namespace danikk_framework
{
	template<class T> struct is_number
	{
		typedef remove_const<T>::type unconst_t;

		static constexpr bool value = std::is_floating_point<T>::value ||
		  std::is_integral<T>::value ||
		  std::is_integral<unconst_t>::value;
	};

	template<class T, class Return> struct enable_if_number
			: public std::enable_if<is_number<T>::value,
			  Return>
	{};

	template<class T, class Return> struct enable_if_not_number
			: public std::enable_if<is_number<T>::value,
			  Return>
	{};
}
