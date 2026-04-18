#include <danikk_framework/log.h>
#include <danikk_framework/filesystem.h>
#include <danikk_framework/datetime.h>
#include <danikk_framework/localization.h>
#include <danikk_framework/static_string.h>
#include <danikk_framework/number.h>
#include <danikk_framework/thread.h>
#include <iostream>

namespace danikk_framework
{
	Mutex logger_mutex;
	FileWriter log_file;
	String log_format_buffer;
	String log_buffer;
	LogConfig log_config;
	uint64 start_time;
	const char* last_log_thread_label = NULL;

	void configureLog()
	{
		start_time = getSystemTimeInMilliseconds();
		if(log_config.log_to_dir)
		{
			log_config.log_to_file = true;
		}
		PathBuffer logsdir;
		logsdir.getExecutableDirectory();
		if(log_config.log_to_file)
		{
			if(log_config.log_to_dir)
			{
				logsdir.pushDirectory("logs");
				if(!exitsFile(logsdir))
				{
					createDirectory(logsdir);
				}
				DateTimeString str = DateTime::local().getString();
				logsdir.setFileName(str.c_string(), "txt");
			}
			else
			{
				logsdir.setFileName("log.txt");
			}
			log_file.open(logsdir);
		}
	}

	static void writeTimeToLogBuffer()
	{
		if(log_config.use_system_time)
		{
			log_buffer << DateTime::local().getString();
		}
		else if(log_config.use_application_time)
		{
			uint64 elapsed_time = getSystemTimeInMilliseconds() - start_time;
			StringBuffer<32> elapsed_buffer;
			elapsed_buffer.resize(writeNumberToStringBuffer(elapsed_time, elapsed_buffer.data(), elapsed_buffer.capacity()));

			log_buffer << n_chars(' ',12 - std::max(elapsed_buffer.size(), (size_t)4));
			StringBuffer<32> num_buffer;
			num_buffer << n_chars('0', elapsed_buffer.size() > 4 ? 0 : 4 - elapsed_buffer.size()) << elapsed_buffer;

			for(index_t i = 0; i < num_buffer.size(); i++)
			{
				if(num_buffer.size() - 3 == i)
				{
					log_buffer << '.';
				}
				log_buffer << num_buffer[i];
			}
		}
		log_buffer << " ";
	}

	static void writeLogBufferToDestination()
	{
		#define LOG_IF_TO(LOG, EXPR) if(EXPR) LOG << log_buffer;
		if(log_config.log_to_clog)
		{
			std::clog << log_buffer.c_string();
		}
		if(log_config.log_to_cout)
		{
			std::cout << log_buffer.c_string();
		}
		if(log_config.log_to_file)
		{
			log_file.write(log_buffer.data(), log_buffer.size());
			log_file.flush();
		}
	}

	bool startLog(const char* severity)
	{
		if(severity == danikk_framework::internal::log_Debug && !IS_DEBUG)//если это дебаг сообщение в релизной версии
		{
			return false;//то не пишет его
		}
		if(last_log_thread_label != thread_label)
		{
			logger_mutex.lock();
			last_log_thread_label = thread_label;
		}
		log_buffer.clear();
		if(log_config.print_time)
		{
			writeTimeToLogBuffer();
		}
		log_buffer << severity << ' ' << thread_label << ' ';
		return true;
	}

	void endLog()
	{
		log_buffer << "\n";
		writeLogBufferToDestination();
		logger_mutex.unlock();
	}

	namespace internal
	{
		const char* log_Debug 	= "DEBUG";
		const char* log_Info	= "INFO";
		const char* log_Warning = "WARN";
		const char* log_Error 	= "ERROR";
		const char* log_Fatal 	= "FATAL";
		const char* log_Trace 	= "TRACE";
		const char* log_Memory 	= "MEMORY";
		const char* log_Assert 	= "ASSERT";
	}
}
