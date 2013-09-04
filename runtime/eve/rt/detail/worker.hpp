#ifdef EVE_RT_WORKER_H

#include <signal.h>
#include <cassert>
#include <cstdlib>

#include <sys/time.h>

#include <iostream>

namespace eve
{
namespace rt
{

THREAD_LOCAL worker* worker::current_worker = NULL;
const uint64_t worker::PARALLELIZE_HEURISTIC = 3;
const int worker::SLEEP_TIMEOUT_MIN = 5;
const int worker::SLEEP_TIMEOUT_MAX = 5120;

void __worker_suspend(int signal)
{
	eve::rt::worker::current()->on_suspend();
}

void __worker_wake(int signal)
{
	/* receiving the signal makes epoll return automaticly so nothing to do here*/
}

void* __worker_thread(void *a)
{
/*	GC_stack_base stack_base;

	GC_get_stack_base(&stack_base);
	GC_register_my_thread(&stack_base);
*/
	worker::current() = static_cast<worker*>(a);
	worker::current()->loop();

/*	GC_unregister_my_thread(); */

	return NULL;
}

eve::rt::worker*& worker::current()
{
	return worker::current_worker;
}

worker::worker(runtime* rt, const size_t& id)
	: sem_suspend(true), sem_resume(true), rt(rt), id(id), failed_steals(0), timeout(worker::SLEEP_TIMEOUT_MIN)
{
	if (this->rt->n_workers != 1)
	{
		do {
			this->next_victim_id = rand() % this->rt->n_workers;
		} while(unlikely(this->next_victim_id == this->id));
	}
}

void worker::start()
{
	signal(SIGUSR1, __worker_suspend);
	signal(SIGUSR2, __worker_wake);
	pthread_create(&this->thread, NULL, __worker_thread, this);
}

void worker::suspend()
{
	assert(worker::current() != this);

	pthread_kill(this->thread, SIGUSR1);
	this->sem_suspend.lock();
}

void worker::resume()
{
	assert(worker::current() != this);
	this->sem_resume.unlock();
}

void worker::wake()
{
	assert(worker::current() != this);
	pthread_kill(this->thread, SIGUSR2);
}

void worker::stop()
{
	assert(worker::current() != this);
	pthread_join(this->thread, NULL);
}

void worker::on_suspend()
{
	// assert(worker::current() == this);

	/* at this time the other thread is already waiting, so post/wait is safe */
	this->sem_suspend.unlock();
	this->sem_resume.lock();
}

void worker::loop()
{
	assert(worker::current() == this);

	/* start threads suspended */
	this->sem_resume.lock();

	while (likely(!this->rt->shuttingdown()))
	{
		/* local work */
		while (likely(this->has_work()))
			this->work();

		if (likely(this->rt->epoll().process() > 0))
		{
			this->failed_steals = 0;
			continue;
		}

		this->steal_work(true);
	}
}

void worker::complete(volatile int& count)
{
	while (count)
	{
		if (likely(this->has_work()))
		{
			this->work();
			continue;
		}

		if (this->rt->epoll().process() > 0)
		{
			this->failed_steals = 0;
			continue;
		}

		this->steal_work(false);		
	}
}

void worker::complete(volatile bool& completed)
{
	while (!completed)
	{
		if (likely(this->has_work()))
		{
			this->work();
			continue;
		}

		if (this->rt->epoll().process() > 0)
		{
			this->failed_steals = 0;
			continue;
		}

		this->steal_work(false);		
	}
}


inline void worker::schedule(eve::rt::task& task)
{
	return this->tasks.push_top(&task);
}

inline bool worker::parallelize() const
{
	return this->tasks.size() < worker::PARALLELIZE_HEURISTIC;
}

inline void worker::work()
{
	eve::rt::task* t = this->tasks.pop_top();
	(*t)();
}

void worker::steal_work(bool blocking)
{
	size_t victim_id;
	eve::rt::task* t;

	victim_id = this->next_victim_id;

	this->next_victim_id = (victim_id + 1) % this->rt->n_workers;

	if (this->next_victim_id == this->id)
		this->next_victim_id = (this->id + 1) % this->rt->n_workers;

	worker& victim = (*this->rt)[victim_id];

	this->lock_local.lock(); // no one can steal from us

	if (blocking && this->failed_steals >= this->rt->n_workers)
	{
		this->lock_local.unlock();

		if (this->rt->shuttingdown())
			return;

		/* go to sleep */
		if (this->rt->epoll().process(this->timeout) > 0)
			this->timeout = worker::SLEEP_TIMEOUT_MIN;
		else
			this->timeout = std::min(this->timeout << 1, worker::SLEEP_TIMEOUT_MAX);

		this->failed_steals = 0;

		return;
	}	

	// heuristic pass
	if (victim.tasks.size() < 2 || unlikely(!victim.lock_local.try_lock()))
	{
		this->failed_steals++;
		this->lock_local.unlock();
		return;
	}

	if (unlikely(this->rt->shuttingdown()))
	{
		/* this check needs to be executed inside the lock and before suspend */
		this->failed_steals++;
		victim.lock_local.unlock();
		this->lock_local.unlock();
		return;
	}

	victim.suspend();

	if (unlikely(victim.tasks.size() < 2))
	{
		this->failed_steals++;

		victim.resume();
		victim.lock_local.unlock();
		this->lock_local.unlock();
		return;
	}
	
	/*
		there is no race condition here because we are the only stealer and the victim is suspended
		also, because there are atleast 2 tasks in the buffer we wont pop the same task the victim has
		(removing from bottom instead of top)
	*/
	t = victim.tasks.pop_bottom();

	this->failed_steals = 0;

	victim.resume();
	victim.lock_local.unlock();
	this->lock_local.unlock();

	(*t)();
}

inline bool worker::has_work()
{
	return !this->tasks.empty();
}

}
}

#endif