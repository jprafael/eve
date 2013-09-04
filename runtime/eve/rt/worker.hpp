#ifndef EVE_RT_WORKER_H
#define EVE_RT_WORKER_H

#include <list>

#include "spinlock.hpp"
#include "platform.hpp"
#include "buffer.hpp"

namespace eve
{
namespace rt
{

class runtime;

class worker
{
	friend class runtime;

	static THREAD_LOCAL worker* current_worker;

	static const uint64_t PARALLELIZE_HEURISTIC;
	static const int SLEEP_TIMEOUT_MIN;
	static const int SLEEP_TIMEOUT_MAX;

	eve::rt::spinlock lock_local;
	eve::rt::spinlock sem_suspend;
	eve::rt::spinlock sem_resume;

	eve::rt::runtime* rt;
	buffer<eve::rt::task*> tasks;

	size_t id;
	pthread_t thread;
	size_t next_victim_id;
	size_t failed_steals;

	int timeout;

	friend eve::rt::buffer<eve::rt::task*>;

public:
	static worker*& current();

	worker(runtime* runtime, const size_t& id = -1);

	/* from outside the thread */
	void start();
	void stop();
	
	/* resume must be called after suspend*/
	void suspend();
	void resume();
	void wake();

	inline bool parallelize() const;

	/* from inside the thread */
	void on_suspend();
	void on_wake();

	void loop();
	inline void schedule(eve::rt::task& task);
	void complete(volatile int& count);
	void complete(volatile bool& completed);

private:
	inline void work();
	inline bool has_work();
	inline void steal_work(bool blocking);
};

}
}

#endif // EVE_WORKER_H
