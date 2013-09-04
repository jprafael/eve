#include <iostream>
#include <vector>
#include <string>

#include <time.h>

#include "../eve/io/socket.hpp"
#include "../eve/io/epoll.hpp"
#include "../eve/eve.hpp"

size_t connections;
size_t messages;
size_t threads;
std::string outfile;

const std::string& host = "localhost";
const std::string& port = "8080";

const size_t size = 6;
struct message : public virtual eve::rt::shareable
{
	const char buffer[size] = { 'h', 'e', 'l', 'l', 'o'};
};

eve::rt::shared_ptr<message> msg = eve::rt::make_shared<message>();

using namespace eve::io;
using namespace eve::rt;

volatile size_t	finished = 0;
volatile long times[1000001] = { 0 };
struct timespec t_start, t_end;

long long diff_time(const struct timespec& t_start, const struct timespec& t_end)
{
	time_t s = t_end.tv_sec - t_start.tv_sec;
	long long ns = t_end.tv_nsec - t_start.tv_nsec;

	return ((long long) s) * 1000000000 + ns;
}

void done();

class cont_cb : public eve::rt::function<void, bool>, public eve::event::callback<connection>
{
	size_t n;
	eve::rt::shared_ptr<eve::io::socket> s;
	eve::rt::shared_ptr<message> reply;
	struct timespec t_send;

	int step;
	eve::rt::shared_ptr<cont_cb> self;

public:
	cont_cb() : n(0), reply(eve::rt::make_shared<message>()), step(0)
	{
	}

	virtual ~cont_cb() { };

	void set_self(eve::rt::shared_ptr<cont_cb>& self)
	{
		this->self = self;
	}

	bool operator () (connection& c)
	{
		this->s = c.socket;
		(*this)(true);
		return true;
	}

	void operator () (bool ok)
	{
		assert(ok);
		
		switch (step)
		{
			case 0: goto _loop; break;
			case 1: goto _write_cb; break;
			case 2: goto _read_cb; break;
		}

_loop:
		if (this->n == messages)
		{
			(*this->s).close();

			if (__sync_add_and_fetch(&finished, 1) == connections)
				done();

			return;
		}

		this->step = 1;
		(*this->s).write(msg, this->self);
		return;

_write_cb:
		/* write_cb */
		assert(ok);

		clock_gettime(CLOCK_MONOTONIC, &this->t_send);

		this->step = 2;
		(*this->s).read(this->reply, this->self);
		return;

_read_cb:
		/* read_cb */
		assert(ok);

		struct timespec t_recv;
        clock_gettime(CLOCK_MONOTONIC, &t_recv);

		long long diff = diff_time(this->t_send, t_recv);
		diff /= 10000; // ns => 10us
		
		if (diff > 1000000)
			diff = 1000000;
		
		__sync_fetch_and_add(times + diff, 1);
		this->n++;

		goto _loop;
	}
};

class connect_task : public task
{

public:
	connect_task() { }

	void operator () ()
	{
		eve::rt::shared_ptr<cont_cb> cb = eve::rt::make_shared<cont_cb>();
		(*cb).set_self(cb);

		socket::connect(host, port, cb);
	}
};

class main_task : public task
{

public:
	main_task() { }

	void operator() ()
	{
		clock_gettime(CLOCK_MONOTONIC, &t_start);

		for (size_t i = 0; i < connections; i++)
		{
			connect_task* c = new connect_task();
			worker::current()->schedule(*c);
		}
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

	double tp = (total_msgs / (total_time/1000000000.0));

	std::cerr << tp << " #/s " << std::endl;
	show_times();

	FILE* file = fopen(outfile.c_str(), "w");
    fwrite(&tp, sizeof(double), 1, file);

    for (int i = 0; i < 1000001; i++)
        fwrite((const void*) &times[i], sizeof(long), 1, file);

    fclose(file);

	runtime::current()->shutdown();
}

int main(int argc, char** argv)
{
	if (argc < 5)
	{
		std::cerr << "./client conn msgs threads outfile" << std::endl;
		return 0;
	}

	connections = atoi(argv[1]);
	messages = atoi(argv[2]);
	threads = atoi(argv[3]);
	outfile = argv[4];

	runtime rt(threads);

	main_task main;
	rt[0].schedule(main);

	rt.startall();
	rt.waitall();

	return 0;
}
