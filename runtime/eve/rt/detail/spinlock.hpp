#ifdef EVE_RT_SPINLOCK_H

#include <sched.h>

namespace eve
{
namespace rt
{

spinlock::spinlock(bool initial)
	: l(initial)
{

}

inline void spinlock::lock()
{
	while (__sync_lock_test_and_set(&this->l, true))
		while (this->l)
			::sched_yield();
}

inline bool spinlock::try_lock()
{
	return !__sync_lock_test_and_set(&this->l, true);
}

inline void spinlock::unlock()
{
	__sync_lock_release(&this->l);
}

}
}

#endif