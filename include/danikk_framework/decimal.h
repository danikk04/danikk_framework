#pragma once

#include<danikk_framework/danikk_framework.h>
#include<danikk_framework/config.h>

namespace danikk_framework
{
	template <class value_type,int32 precision> class Decimal
	{
		static_assert(precision > 0);
	private:
		value_type value = 0;

	public:
		constexpr Decimal() = default;

		constexpr explicit Decimal(value_type raw)
			: value(raw)
		{}

		static constexpr Decimal fromRaw(value_type raw)
		{
			return Decimal(raw);
		}

		static constexpr Decimal fromInt(int32 integer)
		{
			return Decimal(integer * precision);
		}

		constexpr value_type raw() const
		{
			return value;
		}

		constexpr int32 toInt() const
		{
			return value / precision;
		}

		constexpr float toFloat() const
		{
			return static_cast<float>(value) / precision;
		}

		constexpr Decimal operator+(Decimal other) const
		{
			return fromRaw(value + other.value);
		}

		constexpr Decimal operator-(Decimal other) const
		{
			return fromRaw(value - other.value);
		}

		constexpr Decimal operator*(int32_t multiplier) const
		{
			return fromRaw(value * multiplier);
		}

		constexpr Decimal operator/(int32_t divisor) const
		{
			return fromRaw(value / divisor);
		}

		constexpr Decimal& operator+=(Decimal other)
		{
			value += other.value;
			return *this;
		}

		constexpr Decimal& operator-=(Decimal other)
		{
			value -= other.value;
			return *this;
		}

		constexpr bool operator==(Decimal other) const
		{
			return value == other.value;
		}

		constexpr bool operator!=(Decimal other) const
		{
			return value != other.value;
		}

		constexpr bool operator<(Decimal other) const
		{
			return value < other.value;
		}

		constexpr bool operator>(Decimal other) const
		{
			return value > other.value;
		}
	};
}
