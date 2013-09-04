#include <iostream>
#include <sys/time.h>

long long fib(int n)
{
	if (n < 2)
		return n;
	return fib(n-1)+fib(n-2);
}

long long timer(struct timeval &start, struct timeval &end)
{
	long long mtime, secs, usecs;

	secs = end.tv_sec - start.tv_sec;
	usecs = end.tv_usec - start.tv_usec;
	mtime = (secs*1000 + usecs/1000.0) + 0.5;

	return mtime;
}

int main()
{
	struct timeval start, end;

	gettimeofday(&start, NULL);
	std::cout << fib(42) << std::endl;
	gettimeofday(&end, NULL);

	std::cerr << timer(start, end) << std::endl;

	return 0;
}
