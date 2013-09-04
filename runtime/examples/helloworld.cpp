#include <iostream>
#include <vector>
#include <string>

#include "../eve/io/socket.hpp"
#include "../eve/io/epoll.hpp"
#include "../eve/eve.hpp"

using namespace eve::io;

std::string message("Hello World!\n");

class callback_handler : public eve::rt::callback<void, bool>
{

public:
	callback_handler()
	{
	}

	void operator () (bool event)
	{
		std::cerr << "done" << std::endl;
	}
};

class main_task : public eve::rt::task
{
public:
	main_task()
	{
	}

	void operator() ()
	{
		std::shared_ptr<callback_handler> handler = std::make_shared<callback_handler>();
		eve_stdout->object().write(message.c_str(), message.length(), handler);
	}
};

int main()
{
	eve_stdout->object().set_shared((*eve_stdout).as<eve::io::fd>());

	epoll p;

	eve::rt::runtime rt;
	main_task main;

	rt[0].schedule(main);

	rt.startall();
	rt.waitall();

	return 0;
}