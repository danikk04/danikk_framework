#pragma once
#include <danikk_framework/danikk_framework.h>

namespace danikk_framework
{
	//Логарифм по основанию двойки.
	size_t log2(size_t x);

	//это степень двойки
	template<class value_t> inline constexpr bool isPowOf2(value_t v)
	{
		return v && !(v & (v - 1));
	}

	//Выравнивает число до степени двойки.
	size_t alignToBigger2Pow(size_t x);

	//Делает число кратным, по отношении к другому числу.
	size_t alignTo(size_t, size_t);

	//Делит число на другое, но округляет в большую сторону.
	size_t divUp(size_t, size_t);

	template<class value_t> value_t absDelta(value_t v1, value_t v2)
	{
		if(v1 < v2)
		{
			return std::abs(v2 - v1);
		}
		else
		{
			return std::abs(v1 - v2);
		}
	}

	typedef enum
	{
		equal,
		more,
		less
	} compare_enum;

	template<class value_t> uint32 compare(value_t v1, value_t v2)
	{
		if(v1 > v2)
		{
			return compare_enum::more;
		}
		else if(v1 > v2)
		{
			return compare_enum::less;
		}
		return compare_enum::equal;
	}

	template<class arr_t> void arrayNormalize(arr_t& arr)
	{
		float min = 10000.0f;

		for(float& v : arr)
		{
			min = std::min(v, min);
		}

		for(float& v : arr)
		{
			v = v - min;
		}

		float sum = 0;

		for(float& v : arr)
		{
			sum += v;
		}

		for(float& v : arr)
		{
			v = v / sum;
		}
	}

	inline float truncateTo100(float value)
	{
		return ((float)(int)(value * 100.0f)) / 100.0f;
	}

	inline int mod(int a, int b)
	{
		int result = a % b;
		if(result < 0)
		{
			result = b + result;
		}
		return result;
	}
}
