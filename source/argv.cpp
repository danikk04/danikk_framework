#include <danikk_framework/argv.h>
#include <danikk_framework/localization.h>
#include <danikk_framework/log.h>
#include <danikk_framework/cstring_functions.h>
#include <danikk_framework/thread.h>

namespace danikk_framework
{
	extern const char* executablePath;

	namespace argv
	{
		static size_t remaining_argv = 0;
		static char** current_argv;

		void init(int argc,char** argv)
		{
			thread_label = "MAIN";
			executablePath = *argv;
			remaining_argv = argc - 1;
			current_argv = argv + 1;
		}

		void checkInit()
		{
			if(remaining_argv == 0)
			{
				logFatal(localization("call_argv_init"));
				abort();
			}
		}

		char* next()
		{
			checkInit();
			char* result = *current_argv;
			remaining_argv--;
			current_argv++;
			return result;
		}

		size_t remaining()
		{
			return remaining_argv;
		}

		/*void toConfig(Config& cnf)
		{
			if(remaining() > 0)
			{
				logInfo("argv to config:");
			}
			while(remaining() > 0)
			{
				char* current_arg = next();
				if(strstartswith(current_arg, "--"))
				{
					const char* flag_name = current_arg + 2;
					cnf.set(flag_name, (const char*)"1");
					formatLogInfo("key:% value:1", flag_name);
				}
				else if(strstartswith(current_arg, "-"))
				{
					const char* flag_name = current_arg + 1;
					const char* value = next();
					cnf.set(flag_name, value);
					formatLogInfo("key:% value:%", flag_name, value);
				}
			}
		}*/

		void log()
		{
			char** cur = current_argv;
			char** end = current_argv + remaining_argv;
			if(!startLogInfo())
			{
				return;
			}
			while(cur < end)
			{
				log_buffer << *cur << ' ';
				cur++;
			}
			endLog();
		}
	}
}
