#pragma once

#include <danikk_framework/string.h>
#include <danikk_framework/dictionary.h>
#include <functional>
#include "static_string.h"

namespace danikk_framework
{
	struct CLICommand
	{
		string
		std::function<void(StringDictionary)> on_click;
	};

	class CLI
	{

	};
}
