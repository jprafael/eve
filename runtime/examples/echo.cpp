#include <iostream>
#include <vector>
#include <string>

#include "../eve/io/socket.hpp"
#include "../eve/io/epoll.hpp"
#include "../eve/eve.hpp"

using namespace eve::io;
using namespace eve::rt;

class client_handler : public eve::event::callback<eve::io::connection>
{

public:
	bool operator () (eve::io::connection& c)
	{
		eve::io::socket& socket = (*c.socket);
		socket.pipe(socket);
		return true;
	}
};

class listening_handler : public eve::event::callback<eve::io::listening>
{	

public:
	bool operator () (eve::io::listening& c)
	{
		std::cerr << "Listening" << std::endl;
		return true;
	}
};

class main_task : public task
{
	eve::rt::shared_ptr<listening_handler> listening_cb;
	eve::rt::shared_ptr<client_handler> connection_cb;

public:
	main_task()
	{
	}

	void operator() ()
	{
		this->listening_cb = eve::rt::make_shared<listening_handler>();
		this->connection_cb = eve::rt::make_shared<client_handler>();
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
	runtime rt;
	struct timeval start, end;

	main_task main;
	rt[0].schedule(main);

	gettimeofday(&start, NULL);
	rt.startall();
	rt.waitall();
	gettimeofday(&end, NULL);

	std::cerr << timer(start, end) << std::endl;

	return 0;
}