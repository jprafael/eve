#ifdef EVE_RT_MONITOR_H

#include <sched.h>

namespace eve
{
namespace rt
{

/* from: http://locklessinc.com/articles/locks/ */

#define atomic_xadd(P, V) __sync_fetch_and_add((P), (V))
#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))
#define atomic_inc(P) __sync_add_and_fetch((P), 1)
#define atomic_dec(P) __sync_add_and_fetch((P), -1) 
#define atomic_add(P, V) __sync_add_and_fetch((P), (V))
#define atomic_set_bit(P, V) __sync_or_and_fetch((P), 1<<(V))
#define atomic_clear_bit(P, V) __sync_and_and_fetch((P), ~(1<<(V)))

/* Compile read-write barrier */
#define barrier() asm volatile("": : :"memory")

/* Pause instruction to prevent excess processor bus usage */ 
//#define cpu_relax() asm volatile("pause\n": : :"memory")
#define cpu_relax() ::sched_yield()

/* Atomic exchange (of various sizes) */
static inline void *xchg_64(void *ptr, void *x)
{
	__asm__ __volatile__("xchgq %0,%1"
				:"=r" ((unsigned long long) x)
				:"m" (*(volatile long long *)ptr), "0" ((unsigned long long) x)
				:"memory");

	return x;
}

static inline unsigned xchg_32(void *ptr, unsigned x)
{
	__asm__ __volatile__("xchgl %0,%1"
				:"=r" ((unsigned) x)
				:"m" (*(volatile unsigned *)ptr), "0" (x)
				:"memory");

	return x;
}

static inline unsigned short xchg_16(void *ptr, unsigned short x)
{
	__asm__ __volatile__("xchgw %0,%1"
				:"=r" ((unsigned short) x)
				:"m" (*(volatile unsigned short *)ptr), "0" (x)
				:"memory");

	return x;
}

/* Test and set a bit */
static inline char atomic_bitsetandtest(void *ptr, int x)
{
	char out;
	__asm__ __volatile__("lock; bts %2,%1\n"
						"sbb %0,%0\n"
				:"=r" (out), "=m" (*(volatile long long *)ptr)
				:"Ir" (x)
				:"memory");

	return out;
}


#ifndef EVE_RT_MONITOR_TICKET_IMPL
monitor::monitor()
	: bitmap(0)
{
}

inline void monitor::lock_shared()
{
	while (true)
	{
		if (unlikely(__sync_fetch_and_add(&this->bitmap, 2) & 0x0000001))
		{
			/* was locked for writting */
			__sync_fetch_and_sub(&this->bitmap, 2);

			while (this->bitmap & 0x00000001)
				cpu_relax();

			continue;
		}

		return;
	}
}

inline bool monitor::try_lock_shared()
{
	/* wait for the write bit to be clear*/
	if (this->bitmap & 0x00000001)
		return false;

	if (unlikely(__sync_fetch_and_add(&this->bitmap, 2) & 0x0000001))
	{
		/* was locked for writting */
		__sync_fetch_and_sub(&this->bitmap, 2);
		return false;
	}

	return true;
}

inline void monitor::unlock_shared()
{
	__sync_fetch_and_sub(&this->bitmap, 2);
}

inline void monitor::lock_unique()
{
	while (true)
	{
		/* attempt to lock it for ourselves */
		if (unlikely(__sync_fetch_and_or(&this->bitmap, 0x00000001) & 0x00000001))
		{
			while (unlikely(this->bitmap & 0x00000001))
				cpu_relax();

			continue;
		}

		/* locked, now wait for the readers to finish */
		while (unlikely(this->bitmap != 0x00000001))
			cpu_relax();

		return;
	}
}

inline void monitor::unlock_unique()
{
	__sync_and_and_fetch(&this->bitmap, ~0x00000001);
}

inline bool monitor::try_lock_unique()
{
	/* attempt to lock it for ourselves */
	if (__sync_fetch_and_or(&this->bitmap, 0x00000001) & 0x00000001)
		return false;

	/* locked, now wait for the readers to finish */
	while (this->bitmap != 0x00000001)
		cpu_relax();

	return true;
}

#else

monitor::monitor()
//	: ticket.u(0)
{
}

inline void monitor::lock_unique()
{
	unsigned me = atomic_xadd(&this->ticket.u, (1<<16));
	unsigned char val = me >> 16;
	
	while (val != this->ticket.s.write)
		cpu_relax();
}

inline void monitor::unlock_unique()
{
	rwticket t = this->ticket;
	
	barrier();

	t.s.write++;
	t.s.read++;
	
	this->ticket.us = t.us;
}

inline bool monitor::try_lock_unique()
{
	unsigned me = this->ticket.s.users;
	unsigned char menew = me + 1;
	unsigned read = this->ticket.s.read << 8;
	unsigned cmp = (me << 16) + read + me;
	unsigned cmpnew = (menew << 16) + read + me;

	if (cmpxchg(&this->ticket.u, cmp, cmpnew) == cmp)
		return true;
	
	return false;
}

inline void monitor::lock_shared()
{
	unsigned me = atomic_xadd(&this->ticket.u, (1<<16));
	unsigned char val = me >> 16;
	
	while (val != this->ticket.s.read)
		cpu_relax();
	this->ticket.s.read++;
}

inline void monitor::unlock_shared()
{
	atomic_inc(&this->ticket.s.write);
}

inline bool monitor::try_lock_shared()
{
	unsigned me = this->ticket.s.users;
	unsigned write = this->ticket.s.write;
	unsigned char menew = me + 1;
	unsigned cmp = (me << 16) + (me << 8) + write;
	unsigned cmpnew = ((unsigned) menew << 16) + (menew << 8) + write;

	if (cmpxchg(&this->ticket.u, cmp, cmpnew) == cmp)
		return true;
	
	return false;
}

#endif
}
}

#endif