#ifdef EVE_IO_SOCKET_H

#include <cassert>

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <memory.h>

namespace eve
{
namespace io
{

socket::socket(const int& fd)
	: eve::io::fd(fd)
{
}

socket::~socket()
{
}

class socket_connect_handler : public eve::event::callback<writable>
{
	eve::rt::shared_ptr<socket> s;
	eve::rt::shared_ptr<eve::event::callback<connection>> cb;

public:
	socket_connect_handler(eve::rt::shared_ptr<socket>& s, const eve::rt::shared_ptr<eve::event::callback<connection>>& cb)
		: s(s), cb(cb)
	{
	}

	virtual ~socket_connect_handler() {}

	bool operator () (writable& w)
	{
		int value;
		unsigned int len = sizeof(value);

		int err = ::getsockopt((*s)._fd, SOL_SOCKET, SO_ERROR, &value, &len);

		assert(err == 0);
		assert(value == 0);

		eve::io::connection c(s);
		eve::rt::shared_ptr<eve::event::callback<connection>> cb = this->cb;

		eve::event::off<writable>(*s, this);
		(*cb)(c);

		return true;
	}
};

eve::rt::shared_ptr<socket> socket::connect
(
	const std::string& hostname,
	const std::string& port,
	const eve::rt::shared_ptr<eve::event::callback<connection> >& cb_connection
)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int fd = -1;
	int err;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family		= AF_UNSPEC;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_flags		= AI_PASSIVE;

	/* TODO: async lookup */
	do 
		err = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &result);
	while (err == EAI_AGAIN);

	assert(err == 0);

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		fd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (fd < 0)
			continue;

		bool set = fd::set_non_block(fd);
		assert(set);

		int ret = ::connect(fd, rp->ai_addr, rp->ai_addrlen);
		assert(ret == -1);
		if (errno == EINPROGRESS) /* OK! */
		{
			errno = 0;
			break;
		}

		::close(fd);
	}

	assert (rp != NULL);

	freeaddrinfo(result);

	eve::rt::shared_ptr<socket> client = eve::rt::make_shared<socket>(fd);
	socket& s = *client;
	s.set_shared(client);

	eve::rt::shared_ptr<socket_connect_handler> handler = eve::rt::make_shared<socket_connect_handler>(client, cb_connection);
	eve::event::on<writable>(s, handler);

	s.arm();

	return client;
}


server_socket::server_socket(const int& fd)
	: eve::io::socket(fd), _handler(eve::rt::make_shared<server_accept_handler>(*this))
{
}

eve::rt::shared_ptr<server_socket> server_socket::listen
(
	const std::string& port,
	const eve::rt::shared_ptr<eve::event::callback<listening> >& cb_listen,
	const eve::rt::shared_ptr<eve::event::callback<connection> >& cb_connection,
	const int& backlog,
	const bool& reuse
)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int fd = -1;
	int err;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family		= AF_UNSPEC;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_flags		= AI_PASSIVE;

	/* TODO: async lookup */
	do
		err = getaddrinfo(NULL, port.c_str(), &hints, &result);
	while (err == EAI_AGAIN);
	
	assert(err == 0);

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		fd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (fd < 0)
			continue;

		if (reuse)
		{
			int reuse_flag = 1;
			::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_flag, sizeof(int));
		}

		if (::bind(fd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;

		::close(fd);
	}

	freeaddrinfo(result);

	bool set = fd::set_non_block(fd);
	assert(set);

	err = ::listen(fd, backlog);
	assert(err == 0);

	eve::rt::shared_ptr<server_socket> server = eve::rt::make_shared<server_socket>(fd);
	server_socket& s = *server;
	s.set_shared(server);

	eve::event::on<listening>(s, cb_listen);
	eve::event::on<connection>(s, cb_connection);
	eve::event::on<readable>(s, s._handler);

	/* FIXME: schedule this as a new task */
	listening l(server);
	eve::event::emit<listening>(s, l);

	s.arm();

	return server;
}

server_accept_handler::server_accept_handler(eve::io::server_socket& server)
	: server(server)
{
}

bool server_accept_handler::operator() (readable& r)
{
	int fd;
	bool set;

	fd = ::accept(this->server(), NULL, NULL);
	assert(fd >= 0);
	
	set = socket::set_non_block(fd);
	assert(set);

	eve::rt::shared_ptr<eve::io::socket> client = eve::rt::make_shared<eve::io::socket>(fd);
	eve::io::socket& socket = *client;
	socket.set_shared(client);

	(*client).arm();

	connection c(client);
	eve::event::emit<connection>(this->server, c);

	this->server.rearm();

	return true;
}

}
}

#endif