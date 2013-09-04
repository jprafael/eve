#include <iostream>
#include <vector>
#include <string>

#include <time.h>

#include "../eve/io/socket.hpp"
#include "../eve/io/epoll.hpp"
#include "../eve/eve.hpp"

size_t connections = 200;
size_t messages = 10000;

const std::string& host = "localhost";
const std::string& port = "8080";

const size_t size = 6;
struct message
{
	const char buffer[size] = { 'h', 'e', 'l', 'l', 'o'};
};

eve::rt::shared_ptr<message> msg = eve::rt::make_shared<message>();

using namespace eve::io;
using namespace eve::rt;

volatile size_t	 finished = 0;
volatile long times[1000001] = { 0 };
struct timespec t_start, t_end;

long long diff_time(const struct timespec& t_start, const struct timespec& t_end)
{
	time_t s = t_end.tv_sec - t_start.tv_sec;
	long long ns = t_end.tv_nsec - t_start.tv_nsec;

	return ((long long) s) * 1000000000 + ns;
}

void loop (size_t n, eve::rt::shared_ptr<eve::io::socket>& s);

class read_cb : public eve::rt::function<void, bool>
{
	size_t n;
	eve::rt::shared_ptr<eve::io::socket> s;
	eve::rt::shared_ptr<message> reply;
	struct timespec t_send;

public:
	read_cb(size_t n, eve::rt::shared_ptr<eve::io::socket>& s, eve::rt::shared_ptr<message>& reply, const struct timespec& t_send)
		: n(n), s(s), reply(reply), t_send(t_send)
	{
	}

	virtual ~read_cb() {};

	void operator () (bool ok)
	{
		assert(ok);

		struct timespec t_recv;
        clock_gettime(CLOCK_MONOTONIC, &t_recv);

        long long diff = diff_time(t_send, t_recv);
		diff /= 10000; // ns => 10us
		
		if (diff > 1000000)
			diff = 1000000;
		
		__sync_fetch_and_add(times + diff, 1);

		loop(n + 1, s);
	}
};

class write_cb : public eve::rt::function<void, bool>
{
	size_t n;
	eve::rt::shared_ptr<eve::io::socket> s;

public:
	write_cb(size_t n, eve::rt::shared_ptr<eve::io::socket>& s) : n(n), s(s) {}
	virtual ~write_cb() {};

	void operator () (bool ok)
	{
		assert(ok);

		struct timespec t_send;
		clock_gettime(CLOCK_MONOTONIC, &t_send);

		eve::rt::shared_ptr<message> reply = eve::rt::make_shared<message>();
		eve::rt::shared_ptr<read_cb> cb = eve::rt::make_shared<read_cb>(n, s, reply, t_send);

		(*s).read(reply, cb);
	}
};

class connection_cb : public eve::event::callback<connection>
{

public:
	connection_cb() { }
	virtual ~connection_cb() { }

	bool operator () (connection& r)
	{
		loop(0, r.socket);
		return true;
	}
};

class main_task : public task
{

public:
	main_task() { }

	void operator() ()
	{
		clock_gettime(CLOCK_MONOTONIC, &t_start);
		eve::rt::shared_ptr<connection_cb> cb = eve::rt::make_shared<connection_cb>();
		for (size_t i = 0; i < connections; i++)
			socket::connect(host, port, cb);
	}
};

/* report */
void show_times(int start, int stop, int step)
{
    for (int i = start; i < stop; i += step) {
        long sum = 0;
        for (int j = 0; j < step; ++j) sum += times[i+j];
        if (sum > 0) {
            if (start < 99) {
                printf(" <%5d [us]: %ld\n", (i+step)*10, sum);
            } else {
                printf(" <%5d [ms]: %ld\n", (i+step)/100, sum);
            }
        }
    }
}

void show_times()
{
    show_times(0, 10, 1);
    show_times(10, 100, 10);
    show_times(100, 1000, 100);
    show_times(1000, 10000, 1000);
    show_times(10000, 100000, 10000);
    show_times(100000, 1000000, 100000);
    printf(" >= 10sec: %ld\n", times[1000000]);
}

void done()
{
	clock_gettime(CLOCK_MONOTONIC, &t_end);

	/* report */
	size_t total_msgs = connections * messages;
	long long total_time = diff_time(t_start, t_end);

	std::cerr << (total_msgs / (total_time/1000000000.0)) << " #/s " << std::endl;

	show_times();

	runtime::current()->shutdown();
}

void loop (size_t n, eve::rt::shared_ptr<eve::io::socket>& s)
{
	if (n == messages)
		if (__sync_add_and_fetch(&finished, 1) == connections)
			done();

	eve::rt::shared_ptr<write_cb> cb = eve::rt::make_shared<write_cb>(n, s);
	(*s).write(msg, cb);
}

int main(int argc, char** argv)
{
	runtime rt(6);

	main_task main;
	rt[0].schedule(main);

	rt.startall();
	rt.waitall();

	return 0;
}
