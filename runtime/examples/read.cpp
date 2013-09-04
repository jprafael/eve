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
	std::shared_ptr<eve::io::socket> client;

public:
	char data;

	read_cb(std::shared_ptr<eve::io::socket>& client)
		: callback<void, bool>(), client(client)
	{
	}

	~read_cb() { }

	void operator () (bool ok)
	{
		if (ok)
			std::cout << this->data << std::endl;
		else
			std::cerr << "error" << std::endl;

		eve::rt::runtime::current()->shutdown();
	}
};

class client_handler : public eve::event::callback<eve::io::connection>
{

public:
	bool operator () (eve::io::connection& c)
	{
		std::shared_ptr<read_cb> cb = std::make_shared<read_cb>(c.client);
		c.client->read(cb->data, cb);

		return true;
	}
};

class listening_handler : public eve::event::callback<eve::io::listening>
{
	std::shared_ptr<server_socket> server;

public:
	bool operator () (eve::io::listening& c)
	{
		this->server = c.server;
		
		std::cout << "Listening" << std::endl;
		return true;
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
		auto listening_cb = std::make_shared<listening_handler>();
		auto connection_cb = std::make_shared<client_handler>();
		
		std::cout << "Starting" << std::endl;
		server_socket::listen("8080", listening_cb, connection_cb);
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