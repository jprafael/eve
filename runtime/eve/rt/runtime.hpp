#ifndef EVE_RT_RUNTIME_H
#define EVE_RT_RUNTIME_H

#include <vector>
#include <stdint.h>
#include <sys/types.h>

#include "task.hpp"
#include "worker.hpp"

#include "../io/epoll.hpp"

namespace eve
{
namespace rt
{

class runtime
{
	friend class worker;

	volatile bool _shutdown;
	char _padding[CACHE_SIZE - sizeof(bool)];

	eve::io::epoll _epoll;

	unsigned int n_workers;
	std::vector<eve::rt::worker> workers;

public:
	static runtime*& current();

	runtime();
	runtime(unsigned int workers);

	eve::io::epoll& epoll();

	void startall();
	void waitall();

	eve::rt::worker& operator [] (size_t worker);

	void shutdown();

private:
	void init(unsigned int workers);
	bool shuttingdown() const;
};

}
}

#endif