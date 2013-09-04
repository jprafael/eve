#ifdef EVE_UTIL_TIMER_H

namespace eve
{
namespace util
{

timer::timer(const struct itimerspec& time, const eve::rt::shared_ptr<eve::rt::function<void> >& fn)
	: fn(fn)
{
	this->fd() = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK); 
	assert(this->fd.valid());

	timerfd_settime(this->fd(), 0, &time, NULL);

	eve::rt::runtime::current()->epoll().add(eve::io::epoll::read, this->fd, &this->handler);
}

timer::~timer()
{
}

eve::rt::shared_ptr<timer>
timer::make(const struct itimerspec& time, const eve::rt::shared_ptr<eve::rt::function<void> >& fn)
{
	eve::rt::shared_ptr<timer> t = eve::rt::make_shared<timer>(time, fn);
	(*t).handler.set_timer(t);
	return t;
}

struct itimerspec
timer::timeout(time_t seconds, long nanoseconds)
{
	struct itimerspec spec = {
		.it_interval =
		{
			.tv_sec = 0,
			.tv_nsec = 0
		},
		.it_value =
		{
			.tv_sec = seconds,
			.tv_nsec = nanoseconds
		}
	};

	return spec;
}

struct itimerspec
timer::interval(time_t seconds, long nanoseconds)
{
	struct itimerspec spec = {
		.it_interval =
		{
			.tv_sec = seconds,
			.tv_nsec = nanoseconds
		},
		.it_value =
		{
			.tv_sec = 0,
			.tv_nsec = 0
		}
	};
	
	return spec;
}

void 
timer::stop()
{
	/* no need to disable it from epoll */
	this->fd.close();
}

timer_handler::timer_handler()
{
}

void timer_handler::operator() (int events)
{
	uint64_t occurences;
	(*t).fd.read_buffer(&occurences);

	for (uint64_t i = 0; i < occurences; i++)
		(*(*this->t).fn)();
}

void timer_handler::set_timer(const eve::rt::shared_ptr<eve::util::timer>& t)
{
	this->t = t;
}

}
}
#endif