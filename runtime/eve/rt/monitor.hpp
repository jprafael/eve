#ifndef EVE_RT_MONITOR_H
#define EVE_RT_MONITOR_H

#include <memory>
#include "spinlock.hpp"

//#define EVE_RT_MONITOR_TICKET_IMPL

namespace eve
{
namespace rt
{

class monitor
{
	#ifndef EVE_RT_MONITOR_TICKET_IMPL
		/* lower bit = locked for writting */
		/* remaining = running readers */
		volatile unsigned int bitmap;

	#else
		union _rwticket
		{
			unsigned int u;
			unsigned short us;
			__extension__ struct
			{
				unsigned char write;
				unsigned char read;
				unsigned char users;
			} s;
			_rwticket() : u(0) {}
		};
		typedef union _rwticket rwticket;

		rwticket ticket;
	#endif

	char _padding[CACHE_SIZE - sizeof(size_t)];

public:
	monitor();

	inline void lock_shared();
	inline void unlock_shared();
	inline bool try_lock_shared();

	inline void lock_unique();
	inline void unlock_unique();
	inline bool try_lock_unique();
};

}
}

#endif