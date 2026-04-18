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

#include <thread>

namespace danikk_framework
{
	namespace internal
	{
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

		void execr(String& result, const char* cmd)
		{
			FILE* pipe = popen(cmd, "r");
		   	if (pipe == NULL)
		   	{
		   		goto end;
		   	}
		   	result.clear();
		    char buffer[128];
		    while (!feof(pipe)) {
		        if (fgets(buffer, 128, pipe) != NULL)
		        {
		        	result.append(buffer, 128);
		        }
		    }
		    end:
			pclose(pipe);
		    return;
		}

		template<class firstT,class... Types> int exec_file_read(String& result, firstT command, Types... args)
		{
			using namespace internal;

			PathBuffer tempFilePathBuffer;
			tempFilePathBuffer.setPath(getTempDirectory());
			format(result, "temp_cmd_%.data", (size_t)&result);//result будет использован как буффер для пути
			tempFilePathBuffer.setFileName(result);

			result.clear();//result будет использован как буффер для команды.
			result << command;
			writeArgs(result, args...);
			result << " >" << tempFilePathBuffer.c_string();
			result << command_null_output;

			if constexpr(IS_DEBUG)
			{
				formatLogDebug("% : %", localization("exec_cmd"), result);
			}

			int res = system(result.c_string());

			result.clear();

			FileReader reader;
			reader.open(tempFilePathBuffer);
			assert(reader.isOpen());
			result.resize(reader.size());
			reader.read(result.data(), result.size());
			removeFile(tempFilePathBuffer);

			return res;
		}
	}
}
