#include <iostream>
#include <vector>
#include <string>

#include "../eve/io/socket.hpp"
#include "../eve/io/epoll.hpp"
#include "../eve/eve.hpp"

using namespace eve::io;
using namespace eve::rt;

struct guarded : monitor
{
	volatile int a, b;
	guarded() : monitor(), a(0), b(0) {}
};

class write_task : public task
{
	std::shared_ptr<guarded> mon;

public:
	write_task(const std::shared_ptr<guarded>& mon)
		: mon(mon)
	{
	}

	void operator () ()
	{
		for (int i = 0; i < 1000000; i++)
		{
			this->mon->lock_unique();
			this->mon->a++;
			this->mon->b--;
			this->mon->unlock_unique();
		}
	}
};

class read_task : public task
{
	std::shared_ptr<guarded> mon;

public:
	read_task(const std::shared_ptr<guarded>& mon)
		: mon(mon)
	{
	}

	void operator () ()
	{
		for (int i = 0; i < 1000000; i++)
		{
			this->mon->lock_shared();
			int s = this->mon->a + this->mon->b;
			assert(s == 0);
			this->mon->unlock_shared();
		}
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

class main_task : public task
{
	std::vector<std::shared_ptr<guarded>> objs;
	int N, M;
	float write_ratio;

public:
	main_task(int N, int M, float write_ratio)
		: objs(N), N(N), M(M), write_ratio(write_ratio)
	{
		for (int i = 0; i < N; i++)
			objs[i] = std::make_shared<guarded>();
	}

	void operator () ()
	{
		std::vector<task*> t(M);

		for (int i = 0; i < M; i++)
		{
			if ((float)rand() / (float)RAND_MAX < write_ratio)
				t[i] = new write_task(this->objs[i % N]);
			else
				t[i] = new read_task(this->objs[i % N]);
			
			worker::current()->schedule(*t[i]);
		}

		for (int i = 0; i < M; i++)
		{
			worker::current()->complete(*t[i]);
			delete t[i];
		}

		runtime::current()->shutdown();
	}
};

int main(int argc, char** argv)
{
	runtime* rt;
	struct timeval start, end;

	rt = new runtime();

	main_task main(atoi(argv[1]), 100, atof(argv[2]));
	(*rt)[0].schedule(main);

	gettimeofday(&start, NULL);
	rt->startall();
	rt->waitall();
	gettimeofday(&end, NULL);

	std::cerr << timer(start, end) << std::endl;

	delete rt;

	return 0;
}