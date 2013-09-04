#include <iostream>
#include <functional>

#include <sys/time.h>

#include "../eve/event/emitter.hpp"
#include "../eve/eve.hpp"

using namespace std;
using namespace eve::rt;
using namespace eve::event;

struct event_one {};
struct event_two {
	int value;
	event_two(int value) : value(value) {}
};

struct event_three {};

struct complex : public emitter<event_one>, public emitter<event_two>, public emitter<event_three> {};

struct cb1 : eve::event::callback<event_one>
{
public:
	bool operator () (event_one& arg)
	{
		std::cout << "CB1 " << std::endl;
		return true;
	};
};

struct cb2 : eve::event::callback<event_two>
{
public:
	bool operator () (event_two& arg)
	{
		std::cout << "CB2 " << arg.value << std::endl;
		return true;
	};
};

int main(int argc, char** argv)
{
	complex C;
	event_one e;
	event_two e2(1000);

	on<event_one>(C, std::make_shared<cb1>());
	on<event_one>(C, std::make_shared<cb1>());

	emit<event_one>(C, e);

	on<event_two>(C, std::make_shared<cb2>());
	emit<event_two>(C, e2);

	return 0;
}
