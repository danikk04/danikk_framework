#include <danikk_framework/thread.h>
#include <chrono>
#include <stdexcept>

namespace danikk_framework
{
	Task::Task(std::function<void()> function)
		: state(std::make_shared<State>(static_cast<std::function<void()>&&>(function)))
	{
		if(!state->function) throw std::invalid_argument("Task callable must not be empty");
	}

	Task::Task(call_function_t call, void* arg)
	{
		if(call == nullptr) throw std::invalid_argument("Task callback must not be null");
		state = std::make_shared<State>([call, arg]
		{
			call(arg);
		});
	}

	bool Task::isEnd() const
	{
		if(!state) return true;
		std::lock_guard<std::mutex> guard(state->mutex);
		return state->completed;
	}

	void Task::waitFor() const
	{
		if(!state) return;
		std::unique_lock<std::mutex> lock(state->mutex);
		if(!state->submitted && !state->completed)
			throw std::logic_error("Cannot wait for a task that has not been submitted");
		state->completed_condition.wait(lock, [this] { return state->completed; });
		const std::exception_ptr exception = state->exception;
		lock.unlock();
		if(exception) std::rethrow_exception(exception);
	}

	void Task::execute() const noexcept
	{
		if(!state) return;
		std::function<void()> function;
		{
			std::lock_guard<std::mutex> guard(state->mutex);
			if(state->completed) return;
			function = static_cast<std::function<void()>&&>(state->function);
		}

		std::exception_ptr exception;
		try
		{
			function();
		}
		catch(...)
		{
			exception = std::current_exception();
		}
		{
			std::lock_guard<std::mutex> guard(state->mutex);
			state->exception = exception;
			state->completed = true;
		}
		state->completed_condition.notify_all();
	}

	ThreadPool::ThreadPool(size_t worker_count)
	{
		if(worker_count == 0)
		{
			const unsigned int hardware_threads = std::thread::hardware_concurrency();
			worker_count = hardware_threads > 1 ? hardware_threads - 1 : 1;
		}
		configured_worker_count = worker_count;
		workers.reserve(worker_count);
		try
		{
			for(size_t index = 0; index < worker_count; ++index)
				workers.emplace_back(&ThreadPool::runWorker, this);
		}
		catch(...)
		{
			shutdown();
			throw;
		}
	}

	ThreadPool::~ThreadPool() { shutdown(); }

	Task ThreadPool::submit(Task task)
	{
		if(!task.state) throw std::invalid_argument("Cannot submit an empty Task");
		std::unique_lock<std::mutex> pool_lock(mutex);
		if(stopping) throw std::logic_error("Cannot submit to a stopped ThreadPool");
		{
			std::lock_guard<std::mutex> task_lock(task.state->mutex);
			if(task.state->submitted || task.state->completed || !task.state->function)
				throw std::logic_error("Task has already been submitted or completed");
			task.state->submitted = true;
		}
		try
		{
			queued_tasks.push_back(task);
		}
		catch(...)
		{
			std::lock_guard<std::mutex> task_lock(task.state->mutex);
			task.state->submitted = false;
			throw;
		}
		pool_lock.unlock();
		task_available.notify_one();
		return task;
	}

	void ThreadPool::runWorker()
	{
		while(true)
		{
			Task task;
			{
				std::unique_lock<std::mutex> lock(mutex);
				task_available.wait(lock, [this] { return stopping || !queued_tasks.empty(); });
				if(stopping && queued_tasks.empty()) return;
				task = static_cast<Task&&>(queued_tasks.front());
				queued_tasks.pop_front();
			}
			task.execute();
		}
	}

	void ThreadPool::shutdown()
	{
		{
			std::lock_guard<std::mutex> guard(mutex);
			if(workers.empty()) return;
			stopping = true;
		}
		task_available.notify_all();
		for(std::thread& worker : workers)
			if(worker.joinable()) worker.join();
		workers.clear();
	}

	thread_local Thread* this_thread = NULL;

	void Thread::threadEnable(Thread& ref)
	{
		this_thread = &ref;
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	}

	Thread& Thread::thisThread()
	{
		return *this_thread;
	}

	bool Thread::started()
	{
		return joinable();
	}

	void Thread::join()
	{
		static_cast<thread*>(this)->join();
	}

	/*CycleThread::~CycleThread()
	{
		cancellation_request = true;
	}*/



	void Mutex::lock()
	{
		static_cast<std::mutex*>(this)->lock();
		m_locked = true;
	}

	void Mutex::unlock()
	{
		static_cast<std::mutex*>(this)->unlock();
		m_locked = false;
	}

	bool Mutex::locked()
	{
		return m_locked;
	}

	MutexUser::MutexUser(Mutex& ref)
	{
		ptr = &ref;
		ptr->lock();
	}

	MutexUser::~MutexUser()
	{
		if(ptr)
		{
			ptr->unlock();
		}
	}

	MutexUser::operator bool()
	{
		return (bool)ptr;
	}

	void sleep_for(uint32 ms)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}

	thread_local const char* thread_label = "";
	thread_local bool is_main_thread;
}
