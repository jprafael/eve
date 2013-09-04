#ifndef EVE_RT_SPINLOCK_H
#define EVE_RT_SPINLOCK_H

#include "platform.hpp"

namespace eve
{
namespace rt
{

class spinlock
{
	volatile bool l;
	char _padding[CACHE_SIZE - sizeof(bool)];

public:
	spinlock(bool initial = false);
	
	inline void lock();
	inline bool try_lock();

	inline void unlock();
};

}
}

#endif