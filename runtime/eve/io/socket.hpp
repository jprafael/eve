#ifndef EVE_IO_SOCKET_H
#define EVE_IO_SOCKET_H

#include <vector>

#include "fd.hpp"

#include "../event/emitter.hpp"
#include "../rt/shared.hpp"

namespace eve
{
namespace io
{

struct connection;

class socket : public fd
{
	friend fd;
	
public:
	/* needs to be public for make_shared */
	socket(const int& fd);
	virtual ~socket();

	static eve::rt::shared_ptr<socket> connect(
		const std::string& hostname,
		const std::string& port,
		const eve::rt::shared_ptr<eve::event::callback<connection> >& cb_connection);
};

class server_socket;

/* events */
struct listening
{
	eve::rt::shared_ptr<eve::io::server_socket> socket;
	listening(eve::rt::shared_ptr<eve::io::server_socket>& socket) : socket(socket) { };
	eve::rt::shared_ptr<eve::io::server_socket> operator () () { return this->socket; }
};

struct connection
{
	eve::rt::shared_ptr<eve::io::socket> socket;
	connection(eve::rt::shared_ptr<eve::io::socket>& socket) : socket(socket) { };
	eve::rt::shared_ptr<eve::io::socket> operator () () { return this->socket; }
};

/* handlers */
class server_accept_handler : public eve::event::callback<readable>
{
	eve::io::server_socket& server;
	
public:
	server_accept_handler(eve::io::server_socket& server);
	bool operator () (readable& r);
};

class server_socket
	: 	public socket,
		public eve::event::emitter<listening>,
		public eve::event::emitter<connection>
{

protected:
	eve::rt::shared_ptr<server_accept_handler> _handler;

public:
	server_socket(const int& fd);
	static eve::rt::shared_ptr<server_socket> listen(
		const std::string& port,
		const eve::rt::shared_ptr<eve::event::callback<listening> >& cb_listen,
		const eve::rt::shared_ptr<eve::event::callback<connection> >& cb_connection,
		const int& backlog = 1024,
		const bool& reuse = true);
};

}
}

#endif
