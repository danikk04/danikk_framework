#include <danikk_framework/thread.h>
#include <danikk_framework/log.h>
#include <chrono>

namespace danikk_framework
{
	struct WorkerThread
	{
		Thread thread;
		Mutex mutex;
		Mutex extern_mutex;
		Task* target_task = NULL;
	};

	Array<WorkerThread, 0> workers;
	Array<Task*, 32> scheulded_tasks;
	Mutex scheulde_mutex;

	void workerThreadFunction(WorkerThread* worker)
	{
		while(true)
		{
			worker->mutex.lock();
			start_call:
			USE_MUTEX(worker->extern_mutex)
			{
				worker->target_task->call();
				worker->target_task->setEnd();
				worker->target_task = NULL;
			}
			if(scheulded_tasks.size() > 0)
			{
				USE_MUTEX(scheulde_mutex)
				{
					worker->target_task = scheulded_tasks.pop();
					goto start_call;
				}
			}
		}
	}

	Task::Task(Task::call_function_t call, void* arg)
	{
		throw "not work";
		this->function = call;
		this->arg = arg;

		assert(this->function != NULL && this->arg != NULL);

		if(workers.size() == 0)
		{
			workers.resize(workers.capacity());
		}

		for(index_t i = 0; i < workers.size(); i++)
		{
			WorkerThread& worker = workers[i];
			if(!worker.thread.started())
			{
				worker.mutex.lock();
				worker.thread.start(workerThreadFunction, &workers[i]);
			}
			else if(worker.target_task != NULL)
			{
				continue;
			}
			worker.target_task = this;
			worker.mutex.unlock();
			this->worker = &worker;
			return;
		}
		USE_MUTEX(scheulde_mutex)
		{
			scheulded_tasks.push(this);
		}
	}

	void Task::setEnd()
	{
		arg = NULL;
	}

	bool Task::isEnd()
	{
		return arg == NULL;
	}

	void Task::call()
	{
		function(arg);
	}

	void Task::waitFor()
	{
		if(isEnd())
		{
			return;
		}
		while(worker == NULL)
		{
			for(index_t i = 0; i < workers.size(); i++)
			{
				WorkerThread& worker = workers[i];
				if(worker.target_task == NULL)
				{
					worker.target_task = this;
					//worker.mutex.unlock();
					this->worker = &worker;
				}
			}
		}
		WorkerThread& worker_ref = *(WorkerThread*)worker;
		WAIT_FOR_MUTEX(worker_ref.extern_mutex);
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
