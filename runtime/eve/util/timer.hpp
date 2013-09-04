#ifndef EVE_UTIL_TIMER_H
#define EVE_UTIL_TIMER_H

#include "../io/fd.hpp"
#include "../io/epoll.hpp"

#include <sys/timerfd.h>

namespace eve
{
namespace util
{

class timer;

class timer_handler : public eve::io::epoll::handler
{
	eve::rt::shared_ptr<timer> t;

public:
	timer_handler();
	void set_timer(const eve::rt::shared_ptr<timer>& timer);

	virtual void operator() (int events);
};

class timer : public virtual eve::rt::shareable
{
	friend timer_handler;

	eve::io::fd fd;
	timer_handler handler;
	eve::rt::shared_ptr<eve::rt::function<void> > fn;
	
public:
	static struct itimerspec timeout(time_t seconds, long nanoseconds);
	static struct itimerspec interval(time_t seconds, long nanoseconds);

	static eve::rt::shared_ptr<timer> make(const struct itimerspec& time, const eve::rt::shared_ptr<eve::rt::function<void> >& fn);

	timer(const struct itimerspec& time, const eve::rt::shared_ptr<eve::rt::function<void> >& fn);
	~timer();

	void stop();
};

}
}

#endif