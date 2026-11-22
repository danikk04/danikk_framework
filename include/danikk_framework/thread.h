#pragma once

#include <condition_variable>
#include <deque>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

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
		struct State
		{
			mutable std::mutex mutex;
			std::condition_variable completed_condition;
			std::function<void()> function;
			std::exception_ptr exception;
			bool submitted = false;
			bool completed = true;

			State() = default;
			explicit State(std::function<void()> function)
				: function(static_cast<std::function<void()>&&>(function)), completed(false) {}
		};

		using call_function_t = void(*)(void*);
		std::shared_ptr<State> state;

		void execute() const noexcept;
		friend class ThreadPool;

	public:
		/** @brief Creates an already-completed, empty task handle. */
		Task() = default;
		/** @brief Creates a task handle from an owned callable. */
		explicit Task(std::function<void()> function);
		/** @brief Adapts a C-style callback; the pointed argument must outlive the task. */
		Task(call_function_t call, void* arg);
		/** @brief Returns whether execution has completed. */
		bool isEnd() const;
		/** @brief Waits for completion and rethrows any exception raised by the callable. */
		void waitFor() const;
	};

	/** @brief Fixed worker-thread pool that executes submitted tasks and drains on shutdown. */
	class ThreadPool
	{
		std::mutex mutex;
		std::condition_variable task_available;
		std::deque<Task> queued_tasks;
		std::vector<std::thread> workers;
		size_t configured_worker_count = 0;
		bool stopping = false;

		void runWorker();

	public:
		/** @brief Creates an empty pool without starting worker threads. */
		ThreadPool() = default;
		~ThreadPool();
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;

		/** @brief Schedules a task and returns a handle that can be waited on or copied. */
		Task submit(Task task);
		template<typename callable_t,
			typename = std::enable_if_t<!std::is_same_v<std::remove_cvref_t<callable_t>, Task>>>
		Task submit(callable_t&& callable)
		{
			return submit(Task(std::function<void()>(std::forward<callable_t>(callable))));
		}
		/** @brief Waits for queued work to finish, then joins every worker. */
		void shutdown();
		/** @brief Starts the requested number of worker threads; zero leaves the pool without workers. */
		void initialize(size_t worker_count);
		/** @brief Number of worker threads created by this pool. */
		size_t workerCount() const { return configured_worker_count; }
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
