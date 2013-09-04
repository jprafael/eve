#ifdef EVE_RT_RUNTIME_H

namespace eve
{
namespace rt
{

eve::rt::runtime*& runtime::current()
{
	return worker::current()->rt;
}

runtime::runtime()
	: _shutdown(false), _epoll()
{
	this->init(get_cpu_count());
}

runtime::runtime(unsigned int workers)
	: _shutdown(false), _epoll()
{
	this->init(workers);
}

void runtime::init(unsigned int workers)
{
	this->n_workers = workers;
	this->workers.reserve(workers);
	std::vector<worker>::iterator it = this->workers.begin();

	for (size_t i = 0; i < workers; i++)
	{
		this->workers.emplace(it, this, i);
		++it;
	}
}

eve::io::epoll& runtime::epoll()
{
	return this->_epoll;
}

void runtime::startall()
{
	for (size_t i = 0; i < this->workers.size(); i++)
		this->workers[i].start();

	for (size_t i = 0; i < this->workers.size(); i++)
		this->workers[i].sem_resume.unlock();
}

void runtime::waitall()
{
	for (size_t i = 0; i < this->workers.size(); i++)
		this->workers[i].stop();
}

eve::rt::worker& runtime::operator [] (size_t worker)
{
	return this->workers[worker];
}

void runtime::shutdown()
{
	this->_shutdown = true;

	/* shoot (wake) all threads that are sleeping*/
	for (size_t i = 0; i < this->workers.size(); i++)
		if (worker::current() != &this->workers[i])
			this->workers[i].wake();
}

bool runtime::shuttingdown() const
{
	return this->_shutdown;
}

}
}

#endif