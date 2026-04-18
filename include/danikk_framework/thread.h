#pragma once

#include <mutex>
#include <thread>
#include <functional>
#include <danikk_framework/array.h>
#include <danikk_framework/assert.h>
#include <danikk_framework/danikk_framework.h>

#define WAIT_FOR_MUTEX(MUTEX) MUTEX.lock(); MUTEX.unlock();
#define USE_MUTEX(MUTEX) if(danikk_framework::MutexUser __mu__ = danikk_framework::MutexUser(MUTEX))
#define THREAD_CANCELLATION_POINT if(isThreadCancellation()) { threadCancelAccept(); return; }

namespace danikk_framework
{
	extern thread_local const char* thread_label;
	extern thread_local bool is_main_thread;

	class Thread : private std::thread
	{
	protected:
		static void threadEnable(Thread& ref);
	public:
		bool started();

		void join();

		Thread() = default;

		static Thread& thisThread();

		void checkCancellation();

		void isCancellationWait();

		template<typename call_t, typename... args_t> void start(call_t&& call, args_t&&... args)
		{
			threadEnable(*this);
			*reinterpret_cast<thread*>(this) = std::thread(call, args...);
		}
	};

	class Mutex : private std::mutex
	{
		bool64 m_locked = false;
	public:
		Mutex() = default;

		void lock();

		void unlock();

		bool locked();
	};

	/*class CycleThread : private Thread
	{
	public:
		template<typename call_t, typename... args_t> void start(call_t&& call, args_t&&... args)
		{
			threadEnable(*static_cast<Thread*>(this));
			class CycleWrapper
			{
				void start(CycleThread& self, call_t&& call, args_t&&... args)
				{
					while(!self.cancellation_request)
					{
						call(args...);
					}
				}
			};
			*static_cast<thread*>(this) = std::thread(CycleWrapper::start, *this, call, args...);
		}

		~CycleThread();
	};*/

	class Task
	{
	private:
		typedef void(*call_function_t)(void*);
		call_function_t function = NULL;
		void* arg = NULL;
		void* worker = NULL;
	public:
		Task() = default;

		Task(call_function_t call, void* arg);

		void setEnd();

		bool isEnd();

		void call();

		void waitFor();
	};

	class MutexUser
	{
	private:
		Mutex* ptr = NULL;
	public:
		MutexUser() = default;

		MutexUser(Mutex& ref);

		~MutexUser();

		operator bool();
	};

	void sleep_for(uint32 ms);
}
