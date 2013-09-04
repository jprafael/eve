#include <iostream>

#include <sys/time.h>
#include "../eve/eve.hpp"

using namespace std;
using namespace eve::rt;

long long fib(int n);

class fib_task : public task
{
	long long& ret;
	int n;

public:
	fib_task(long long& ret, const int& n)
		: ret(ret), n(n)
	{
	}

	void operator () ()
	{
		ret = fib(this->n);
	}
};

inline long long fib(int n)
{
	if (n < 2)
		return n;

	if (!worker::current()->parallelize())
		return fib(n - 1) + fib(n - 2);

	long long a, b;

	/* schedule task */
	fib_task t(b, n - 2);
	worker::current()->schedule(t);

	/* do some work meanwhile */
	a = fib(n - 1);

	/* enforce completion of the task */
	worker::current()->complete(t);

	return a + b;
}

class main_task : public task
{
public:
	void operator () ()
	{
		std::cout << fib(40) << std::endl;
		runtime::current()->shutdown();
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
	struct timeval start, end;
	runtime* rt;

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
