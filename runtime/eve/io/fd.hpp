#ifndef EVE_IO_FD_H
#define EVE_IO_FD_H

#include "../rt/function.hpp"
#include "../rt/shared.hpp"
#include "../event/emitter.hpp"

namespace eve
{
namespace io
{

class base_fd
{

public:
	/* this should be protected */
	int _fd;

protected:
	explicit base_fd(const int& fd = -1);

	friend class shared_fd;

public:
	/* this should be protected */
	static bool set_non_block(const int& fd);

public:
	virtual ~base_fd();

	int& operator () ();
	const int& operator () () const;

	bool valid() const;
	virtual void close();
};

/* forward declarations */
class epoll;
class fd;

/* events */
class readable {};
class writable {};
class closed {};

/* using epoll::handler here is not allowed becaused epoll is only a forward declaration */
class fd_epoll_handler : public eve::rt::function<void, int>
{
	eve::io::fd& fd;

public:
	fd_epoll_handler(eve::io::fd& fd);
	void operator() (int event);
};

class fd
	:	public eve::io::base_fd,
		public virtual eve::rt::shareable,
		public event::emitter<readable>,
		public event::emitter<writable>,
		public event::emitter<closed>
{
	fd_epoll_handler _handler;
	eve::rt::shared_ptr<fd> _shared;

private:
	/* disable copy constructor */
	fd(const fd& other) = delete; 

protected:
	int epoll_flag() const;

public:
	explicit fd(const int& fd = -1);
	virtual ~fd();

	template <class K>
	void read(eve::rt::shared_ptr<K>& data, const eve::rt::shared_ptr<eve::rt::function<void, bool> >& cb = eve::rt::function_default<bool>::fn);

	template <class K>
	void write(const K& data, const eve::rt::shared_ptr<eve::rt::function<void, bool> >& cb = eve::rt::function_default<bool>::fn);

	template <class K>
	void write(const eve::rt::shared_ptr<K>& data, const eve::rt::shared_ptr<eve::rt::function<void, bool> >& cb = eve::rt::function_default<bool>::fn);

	void pipe(eve::io::fd& write_fd);

	void arm();
	void rearm();

	void close();

	/* these should be protected */
	eve::rt::shared_ptr<fd>& shared();
	void set_shared(const eve::rt::shared_ptr<fd>& shared);

	fd_epoll_handler& handler();

	bool read_buffer(std::vector<char>& buffer);

	template <class K>
	bool read_buffer(K* buffer);

	void write_buffer(const std::vector<char>& buffer);
};

}
}
#endif