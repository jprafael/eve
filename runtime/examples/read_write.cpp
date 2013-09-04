#include <iostream>
#include <vector>
#include <string>

#include "../eve/io/socket.hpp"
#include "../eve/io/epoll.hpp"
#include "../eve/eve.hpp"

using namespace eve::io;
using namespace eve::rt;

class write_cb : public callback<bool>
{
	tcp_socket* client;

public:
	write_cb(tcp_socket* client)
		: callback<bool>(), client(client)
	{
	}

	~write_cb() { }

	void operator () (bool ok)
	{
		eve::rt::runtime::current()->shutdown();
		delete this->client;
	}
};

class read_cb : public callback<bool>
{
	tcp_socket* client;

public:
	char data;

	read_cb(tcp_socket* client)
		: callback<bool>(), client(client)
	{
	}

	~read_cb() { }

	void operator () (bool ok)
	{
		if (ok)
			std::cout << this->data << std::endl;
		else
			std::cerr << "error" << std::endl;

		
	}
};

class accept_handler : public eve::io::epoll::handler
{
	tcp_socket server;

public:
	accept_handler(tcp_socket& server)
		: server(server)
	{
	}

	void operator () (int events)
	{
		tcp_socket* client = new tcp_socket();

		if (this->server.accept(*client))
		{
			read_cb* cb = new read_cb(client);
			client->read(&cb->data, 1024 * 1024, cb);
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
		tcp_socket server = tcp_socket::listen("8080");

		if (!server.valid())
			return;

		accept_handler* handler = new accept_handler(server);
		runtime::current()->epoll().add(epoll::read, server, handler);
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