#ifdef EVE_IO_EPOLL_H

#include <sys/epoll.h>

namespace eve
{
namespace io
{
	
epoll::epoll()
	: base_fd(epoll_create1(0))
{
	assert((*this)() >= 0);
}

epoll::~epoll()
{
	eve::io::base_fd::close();
}

void epoll::add(int events, const eve::io::fd& fd, handler* f)
{
	int err;
	struct epoll_event event;

	event.events = events | EPOLLONESHOT;
	event.data.ptr = f;

	err = epoll_ctl((*this)(), EPOLL_CTL_ADD, fd(), &event);
	assert(err == 0);
}

void epoll::mod(int events, const eve::io::fd& fd, handler* f)
{
	int err;
	struct epoll_event event;

	event.events = events | EPOLLONESHOT;
	event.data.ptr = f;

	err = epoll_ctl((*this)(), EPOLL_CTL_MOD, fd(), &event);
	assert(err == 0);
}

void epoll::del(const eve::io::fd& fd)
{
	int err = epoll_ctl((*this)(), EPOLL_CTL_DEL, fd(), NULL);
	assert(err == 0);
}

THREAD_LOCAL struct epoll_event epoll_buffer[epoll::max_events];

int epoll::process(const int& timeout)
{
	int err = 0;

	err = epoll_wait((*this)(), epoll_buffer, epoll::max_events, timeout);

	if (err == -1 and errno == EINTR) /* the blocking call got interrupted */
		goto cleanup;

	assert(err >= 0);
	assert(err <= epoll::max_events);

	for (int i = 0; i < err; i++)
	{
		handler& f = *(static_cast<handler*>(epoll_buffer[i].data.ptr));
		f(epoll_buffer[i].events);
	}

cleanup:
	return err;
}

}
}
#endif