#include <execinfo.h>
#include <csignal>
#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/log.h>

namespace danikk_framework
{
	void trace()
	{
		void* callstack[256];
		size_t frame_count = backtrace(callstack, 256);
		char** stack_trace_array = backtrace_symbols(callstack, frame_count);
		for(size_t i = 0; i < frame_count; i++)
		{
			logTrace(stack_trace_array[i]);
		}
	}

	static void tracer(int code)
	{
		std::signal(code, SIG_DFL);
		switch(code)
		{
			#define CASE(CODE) case CODE: logFatal(#CODE); break;
			CASE(SIGABRT);
			CASE(SIGSEGV);
		}
		trace();
		std::raise(code);
	}

	void traceSegfault()
	{
		std::signal(SIGABRT, tracer);
		std::signal(SIGSEGV, tracer);
	}
}
