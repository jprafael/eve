#include <iostream>
#include <vector>
#include <string>

#include "../eve/io/socket.hpp"
#include "../eve/io/epoll.hpp"
#include "../eve/eve.hpp"

using namespace eve::io;

class accept_handler : public eve::rt::callback<int>
{
	tcp_socket& server;
	std::vector<unsigned char>& buffer;

public:
	accept_handler(tcp_socket& server, std::vector<unsigned char>& buffer)
		: server(server), buffer(buffer)
	{
	}

	void operator () (int event)
	{
		tcp_socket client = this->server.accept();
		client.write(this->buffer);
		client.close();
	}
};

int main()
{
	epoll p;
	tcp_socket server = tcp_socket::listen("8080");
	std::string message("Hello World!\n");
	std::vector<unsigned char> buffer(message.begin(), message.end());

	if (!server.valid())
		return 0;

	accept_handler handler(server, buffer);
	p.add(epoll::read, server, &handler);

	while (true)
		p.process();

	return 0;
}