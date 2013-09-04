#ifdef EVE_IO_FD_H

#include <fcntl.h>
#include <sys/socket.h>
#include <memory.h>

#include "../../rt/task.hpp"
#include "../../rt/shared.hpp"

#include <string>

namespace eve
{
namespace io
{

base_fd::base_fd(const int& fd)
	: _fd(fd)
{
}

base_fd::~base_fd()
{
}

int& base_fd::operator () ()
{
	return this->_fd;
}

const int& base_fd::operator () () const
{
	return this->_fd;
}

bool base_fd::set_non_block(const int& fd)
{
	int flags, s;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;

	flags |= O_NONBLOCK;
	s = fcntl(fd, F_SETFL, flags);
	if (s == -1)
		return false;

	return true;
}

bool base_fd::valid() const
{
	return this->_fd >= 0;
}

void base_fd::close()
{
	::close(this->_fd);
	this->_fd = -this->_fd;
}

/*
	FD
*/
fd::fd(const int& fd)
	: base_fd(fd), _handler(*this), _shared(NULL)
{
}

fd::~fd()
{
}

template <class T>
class read_handler : public eve::event::callback<readable>
{
	eve::io::fd& fd;
	eve::rt::shared_ptr<T> buffer;
	ssize_t remaining;
	ssize_t available;

	eve::rt::shared_ptr<eve::rt::function<void, bool> > cb;

public:
	read_handler(eve::io::fd& fd, eve::rt::shared_ptr<T>& buffer, const eve::rt::shared_ptr<eve::rt::function<void, bool> >& cb)
		: fd(fd), buffer(buffer), remaining(sizeof(T)), available(0), cb(cb)
	{
	}

	bool operator () (readable& r)
	{
		ssize_t err;
		bool result = false;

		char *pos = ((char*) (T*) this->buffer) + this->available;
		err = ::recv(this->fd(), pos, this->remaining, 0);

		if (err == this->remaining)
		{
			/* OK */
			result = true;
			goto done;

		} else if (err <= 0 && errno != EAGAIN && errno != EWOULDBLOCK)
		{
			/* got an error */
			goto done;
		}

		/* partial read */
		if (err > 0)
		{
			this->remaining -= err;
			this->available += err;
		}

		/* partial read or EAGAIN */ 
		this->fd.rearm();
		return true;

done:
		/* off<readable> must go before the cb call, but this will be free'd with off, so save a copy of local variables in the stack */ 
		eve::rt::shared_ptr<eve::rt::function<void, bool> > cb = this->cb;
		eve::io::fd& fd = this->fd;

		eve::event::off<readable>(fd, this);
		(*cb)(result);

		/* there are more callbacks waiting for events */
		if (eve::event::has_callback<readable>(fd))
			fd.rearm();

		return true;
	}
};

template <class K>
void fd::read(eve::rt::shared_ptr<K>& data, const eve::rt::shared_ptr<eve::rt::function<void, bool> >& cb)
{
	eve::event::on<readable>(*this, eve::rt::make_shared<read_handler<K>>(*this, data, cb));
	this->rearm();
}

bool fd::read_buffer(std::vector<char>& buffer)
{
	ssize_t err;

	err = ::read(this->_fd, &buffer[0], buffer.capacity());

	// connection closed or no more data
	if (err == 0 || (err == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == ECONNRESET)))
	{
		buffer.resize(0);
		return false;
	}

	assert(err >= 0);
	buffer.resize(err);

	return true;
}

template <typename K>
bool fd::read_buffer(K* buffer)
{
	ssize_t err;

	err = ::read(this->_fd, buffer, sizeof(K));

	// connection closed or no more data
	if (err == 0 || (err == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == ECONNRESET)))
		return false;

	assert(err == sizeof(K));

	return true;
}

void fd::write_buffer(const std::vector<char>& buffer)
{
	ssize_t err;
	if (!buffer.empty())
	{
		err = ::write(this->_fd, &buffer[0], buffer.size());
		assert(err >= 0);
	}
}

/* see code/compiler/runtime/types.hpp for a specialization for eve_string */
template <class T>
class write_handler : public eve::event::callback<writable>
{
	eve::io::fd& fd;
	const eve::rt::shared_ptr<T> buffer;
	ssize_t remaining;
	ssize_t written;

	eve::rt::shared_ptr<eve::rt::function<void, bool> > cb;

public:
	write_handler(eve::io::fd& fd, const eve::rt::shared_ptr<T>& buffer, const eve::rt::shared_ptr<eve::rt::function<void, bool> >&cb)
		: fd(fd), buffer(buffer), remaining(sizeof(T)), written(0), cb(cb)
	{
	}

	bool operator () (writable& w)
	{
		ssize_t err;
		bool result = false;

		const char* pos = ((const char*) (const T*) this->buffer) + this->written;
		err = ::write(this->fd(), pos, this->remaining);

		if (err == this->remaining)
		{
			result = true;
			goto done;

		} else if (err <= 0 && errno != EAGAIN && errno != EWOULDBLOCK)
			goto done;

		/* partial read */
		if (err > 0)
		{
			this->remaining -= err;
			this->written += err;
		}

		/* clear error state */
		errno = 0;

		/* partial read or EAGAIN/EINTR/etc... */
		this->fd.rearm();
		return true;

done:
		/* off<writable> must go before the cb call, but this will be free'd with off, so save a copy of local variables in the stack */ 
		eve::rt::shared_ptr<eve::rt::function<void, bool> > cb = this->cb;
		eve::io::fd& fd = this->fd;

		eve::event::off<writable>(fd, this);
		(*cb)(result);

		/* there are more callbacks waiting for events */
		if (eve::event::has_callback<writable>(fd))
			fd.rearm();

		return true;
	}
};

template <class K>
void fd::write(const eve::rt::shared_ptr<K>& data, const eve::rt::shared_ptr<eve::rt::function<void, bool> >& cb)
{
	eve::event::on<writable>(*this, eve::rt::make_shared<write_handler<K>>(*this, data, cb));
	this->rearm();
}

template <class K>
void fd::write(const K& data, const eve::rt::shared_ptr<eve::rt::function<void, bool> >& cb)
{
	this->write<K>(eve::rt::make_shared<K>(data), cb);
}

/*
void fd::write(const void* data, const ssize_t& size, const eve::rt::shared_ptr<eve::rt::function<void, bool> >& cb)
{
	eve::rt::shared_ptr<void*> copy = eve::rt::make_shared<void*>(new char[size]);
	memcpy(*copy, data, size);

	eve::event::on<writable>(*this, eve::rt::make_shared<write_handler>(*this, copy, size, cb));
	this->arm_or_rearm();
}
*/

class pipe_handler : public eve::event::callback<readable>
{
	eve::io::fd &in, &out;
	std::vector<char> buffer;

public:
	pipe_handler(eve::io::fd& in, eve::io::fd& out)
		: eve::event::callback<readable>(), in(in), out(out), buffer(1024)
	{
	}

	virtual ~pipe_handler() { }

	bool operator () (readable& r)
	{
		while (in.read_buffer(buffer))
			out.write_buffer(buffer);

		in.rearm();

		return true;
	}
};

void fd::pipe (eve::io::fd& other)
{
	eve::event::on<readable>(*this, eve::rt::make_shared<pipe_handler>(*this, other));
	this->rearm();
}

void fd::arm()
{
	assert(this->valid());
	eve::rt::runtime::current()->epoll().add(this->epoll_flag(), *this, &this->_handler);
}

void fd::rearm()
{
	assert(this->valid());

	eve::rt::runtime::current()->epoll().mod(this->epoll_flag(), *this, &this->_handler);
}

int fd::epoll_flag() const
{
	/* always listen for the closed event so we can free resources even if there is no user defined callback */
	int events = eve::io::epoll::close;
	
	if (eve::event::has_callback<readable>(*this))
		events |= eve::io::epoll::read;

	if (eve::event::has_callback<writable>(*this))
		events |= eve::io::epoll::write;

	return events;
}

void fd::close()
{
	eve::event::off<readable>(*this);
	eve::event::off<writable>(*this);
	eve::event::off<closed>(*this);

	base_fd::close();
}

eve::rt::shared_ptr<eve::io::fd>& fd::shared()
{
	assert(this->_shared);
	return this->_shared;
}

void fd::set_shared(const eve::rt::shared_ptr<eve::io::fd>& shared)
{
	assert(!this->_shared);
	this->_shared = shared;
}

fd_epoll_handler& fd::handler()
{
	return this->_handler;
}

fd_epoll_handler::fd_epoll_handler(eve::io::fd& fd)
	: fd(fd)
{
}

void fd_epoll_handler::operator () (int event)
{
	eve::rt::shared_ptr<eve::io::fd>& shared = this->fd.shared();

	shared.monitor().lock_unique();

	if (event & eve::io::epoll::read)
	{
		readable r;
		eve::event::emit<readable>(this->fd, r);
	}

	if (event & eve::io::epoll::write)
	{
		writable w;
		eve::event::emit<writable>(this->fd, w);
	}

	if (event & eve::io::epoll::close)
	{
		this->fd.close();

		closed c;
		eve::event::emit<closed>(this->fd, c);
	}

	shared.monitor().unlock_unique();
}

}
}

#endif