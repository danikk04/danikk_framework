#pragma once

#include<danikk_framework/danikk_framework.h>
#include<danikk_framework/config.h>
#include "path_buffer.h"

namespace danikk_framework
{
	namespace argv
	{
		void log();

		void init(int argc,char** argv);
	}

	#define ARGV_INIT() danikk_framework::argv::init(argc, argv);
}
