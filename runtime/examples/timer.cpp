#include <iostream>
#include <vector>
#include <string>

#include "../eve/io/socket.hpp"
#include "../eve/io/epoll.hpp"
#include "../eve/eve.hpp"
#include "../eve/util/timer.hpp"

using namespace eve::io;
using namespace eve::rt;
using namespace eve::util;

class timeout_handler : public timer::handler
{

public:
	timeout_handler()
	{
	}

	void operator() (timer& t)
	{
		std::cout << "Finished!" << std::endl;

		delete t;
		delete this;
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
		new timer(timer::timeout(1, 0), new timeout_task());
	}
};

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

	rt->startall();
	rt->waitall();

	delete rt;

	return 0;
}