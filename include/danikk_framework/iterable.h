#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/math.h>

namespace danikk_framework
{
	template<class t>struct PointerIterable
	{
		t* _begin;
		t* _end;

		t* begin()
		{
			return _begin;
		}

		t* end()
		{
			return _end;
		}
	};
}
