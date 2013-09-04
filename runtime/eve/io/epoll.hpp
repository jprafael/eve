#ifndef EVE_IO_EPOLL_H
#define EVE_IO_EPOLL_H

#include <sys/epoll.h>
#include "fd.hpp"

namespace eve
{
namespace io
{

class epoll : public base_fd
{

public:
	static const int read = EPOLLIN;
	static const int write = EPOLLOUT;
	static const int close = EPOLLRDHUP | EPOLLHUP | EPOLLERR;
	static const int all = read | write | close;
	static const int max_events = 1024;
	
	epoll();
	~epoll();

	typedef eve::rt::function<void, int> handler;

	void add(int events, const fd& fd, handler* f);
	void mod(int events, const fd& fd, handler* f);
	void del(const fd& fd);

	int process(const int& timeout = 0);
};

}
}

#endif