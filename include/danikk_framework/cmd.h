#pragma once

#include <danikk_framework/static_string.h>
#include <danikk_framework/string.h>
#include <danikk_framework/filesystem.h>
#include <danikk_framework/filestream.h>
#include <danikk_framework/cstring_functions.h>
#include <danikk_framework/number.h>
#include <danikk_framework/format.h>
#include <danikk_framework/log.h>
#include <danikk_framework/localization.h>

#include <cstdio>
#include <csignal>
#include <cstring>
#include <thread>

namespace danikk_framework
{
	namespace internal
	{
		inline volatile std::sig_atomic_t system_close_signal_received = 0;

		inline void handleSystemCloseSignal(int)
		{
			system_close_signal_received = 1;
		}

		#if IS_LINUX
			inline const char* command_null_output = " 2>/dev/null";
		#else
			#error not implemented
		#endif

		inline static void writeArgs(String& buffer)
		{
			(void)buffer;
		}

		template<class firstT,class... Types> inline static void writeArgs(String& buffer, firstT arg,Types... args)
		{
			formatAppend(buffer, " %", single_quotes(arg));
			writeArgs(buffer, args...);
		}
	}

	namespace cmd
	{
		inline bool checkSystemCloseSig()
		{
			static const bool handlers_installed = []()
			{
				std::signal(SIGINT, internal::handleSystemCloseSignal);
				std::signal(SIGTERM, internal::handleSystemCloseSignal);
				return true;
			}();
			(void)handlers_installed;
			return internal::system_close_signal_received != 0;
		}

		template<class firstT,class... Types> int exec(String& output, firstT command,Types... args)
		{
			using namespace internal;
			String command_buffer;
			command_buffer << command;
			writeArgs(command_buffer, args...);

			if constexpr(IS_DEBUG)
			{
				formatLogDebug("% : %", localization("exec_cmd"), command_buffer);
			}

			output.clear();
			FILE* pipe = popen(command_buffer.c_string(), "r");
			if(pipe == NULL)
			{
				return -1;
			}

			char buffer[128];
			while(fgets(buffer, sizeof(buffer), pipe) != NULL)
			{
				output.append(buffer, std::strlen(buffer));
			}

			return pclose(pipe);
		}

		template<class command_t> int exec(const command_t& command)
		{
			if constexpr(IS_DEBUG)
			{
				formatLogDebug("% : %", localization("exec_cmd"), command);
			}

			return system(command.c_string());
		}

		template<class firstT,class... Types> int exec(firstT command,Types... args)
		{
			using namespace internal;
			String command_buffer;
			command_buffer << command;
			writeArgs(command_buffer, args...);

			if constexpr(IS_DEBUG)
			{
				formatLogDebug("% : %", localization("exec_cmd"), command_buffer);
			}

			return system(command_buffer.c_string());
		}

	}
}
