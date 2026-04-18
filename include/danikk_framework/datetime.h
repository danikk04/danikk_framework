#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/cstring_functions.h>
#include <danikk_framework/static_string.h>

namespace danikk_framework
{
	typedef StringBuffer<24> DateTimeString;

	struct DateTime
	{
		static constexpr size_t date_time_string_size = 19;

		uint16 year;
		uint8 month;
		uint8 day;
		uint8 hour;
		uint8 minute;
		uint8 second;

		uint8 error = false;

		void parse(const String& data);

		void readFrom(const char* data);

		DateTimeString getString(char date_separator = '-', char datetime_separator = ' ', char time_separator = ':') const;

		DateTimeString getDateString(char date_separator = '-') const;

		static DateTime local();
	};

	bool operator <(const DateTime& first, const DateTime& second);

	bool operator >(const DateTime& first, const DateTime& second);

	bool operator >=(const DateTime& first, const DateTime& second);

	bool operator <=(const DateTime& first, const DateTime& second);

	uint64 getLocalSecondsTimestamp();

	uint64 getLocalMinutesTimestamp();

	uint64 getSystemTimeInMilliseconds();
}
