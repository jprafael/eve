#include <iostream>
#include <functional>

#include <sys/time.h>

#include "../eve/eve.hpp"

#include <vector>

using namespace std;
using namespace eve::rt;

class main_task : public task
{

public:

	void operator () ()
	{
		{
			std::cerr << "A" << std::endl;
			eve::rt::shared_ptr<int> p = eve::rt::make_shared<int>(4);

			std::cerr << "B" << std::endl;
			std::vector<eve::rt::shared_ptr<int> > A;
			std::cerr << "D" << std::endl;
			A.push_back(p);
		}
		std::cerr << "F" << std::endl;

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
	runtime rt(1);

	main_task main;

	rt[0].schedule(main);

	gettimeofday(&start, NULL);
	rt.startall();
	rt.waitall();
	gettimeofday(&end, NULL);

	std::cerr << timer(start, end) << std::endl;

	return 0;
}
