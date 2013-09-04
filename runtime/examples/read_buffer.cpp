#include <iostream>
#include <vector>
#include <string>

#include "../eve/io/socket.hpp"
#include "../eve/io/epoll.hpp"
#include "../eve/eve.hpp"

using namespace eve::io;
using namespace eve::rt;

class read_cb : public callback<void, bool>
{
	tcp_socket* client;

public:
	char data[33];

	read_cb(tcp_socket* client)
		: callback<void, bool>(), client(client)
	{
	}

	~read_cb() { }

	void operator () (bool ok)
	{
		this->data[32] = 0;
		
		if (ok)
			std::cout << this->data << std::endl;
		else
			std::cerr << "error" << std::endl;

		eve::rt::runtime::current()->shutdown();

		delete this->client;
	}
};

class accept_handler : public eve::io::epoll::handler
{
	tcp_socket* server;

public:
	accept_handler(tcp_socket* server)
		: server(server)
	{
	}

	void operator () (int events)
	{
		tcp_socket* client = new tcp_socket();

		if (this->server->accept(*client))
		{
			std::shared_ptr<read_cb> cb = std::make_shared<read_cb>(client);
			client->read(cb->data, 32, cb);
		}
	}
};

class main_task : public task
{
public:
	main_task()
	{
	}

	void operator() ()
	{
		tcp_socket* server = tcp_socket::listen("8080");

		if (!server->	valid())
			return;

		accept_handler* handler = new accept_handler(server);
		runtime::current()->epoll().mod(epoll::read, *server, handler);
	}
};

long long timer(struct timeval &start, struct timeval &end)
{
	long long mtime, secs, usecs;

	secs = end.tv_sec - start.tv_sec;
	usecs = end.tv_usec - start.tv_usec;
	mtime = (secs*1000 + usecs/1000.0) + 0.5;

	return mtime;
}

int main(int argc, char** argv)
{
	runtime* rt;
	struct timeval start, end;

	if (argc < 2)
		rt = new runtime();
	else
		rt = new runtime(atoi(argv[1]));

	main_task main;

	(*rt)[0].schedule(main);

	gettimeofday(&start, NULL);
	rt->startall();
	rt->waitall();
	gettimeofday(&end, NULL);

	std::cerr << timer(start, end) << std::endl;

	delete rt;

	return 0;
}