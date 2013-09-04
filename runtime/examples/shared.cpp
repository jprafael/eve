#include <iostream>
#include <functional>

#include <sys/time.h>

#include "../eve/eve.hpp"

using namespace std;
using namespace eve::rt;

class Base
{
public:
	virtual ~Base()
	{
		std::cout << "~Base()" << std::endl;
	}
};

class Derived : Base
{
public:
	virtual ~Derived()
	{
		std::cout << "~Derived()" << std::endl;
	}
};

class a_task : public task
{
public:
	void operator () ()
	{
		// simple shared object
/*		eve::rt::shared_ptr<int> a = eve::rt::make_shared<int>(10);
		std::cout << "A " << a.object() << std::endl;

		// assignment to other ptr
		eve::rt::shared_ptr<int> b;
		b = a;
		std::cout << "B " << b.object() << std::endl;

		// assignment to the same ptr
		a = a;
		std::cout << "A " << b.object() << std::endl;

		// assignment to ptr of other type
		eve::rt::shared_ptr<short> c;
		c = a;
		std::cout << "C " << b.object() << std::endl;
*/

		// testing destructors
		eve::rt::shared_ptr<Base> p = eve::rt::make_shared<Base>();
		eve::rt::shared_ptr<Base> d = eve::rt::make_shared<Derived>();

		runtime::current()->shutdown();
	}
};

class main_task : public task
{
public:
	void operator () ()
	{
		const int N = 1;

		std::vector<task*> t(N);

		for (int i = 0; i < N; i++)
		{
			t[i] = new a_task();
			worker::current()->schedule(*t[i]);
		}

		for (int i = 0; i < N; i++)
		{
			worker::current()->complete(*t[i]);
			delete t[i];
		}

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
