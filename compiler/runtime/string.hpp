class eve_string : public std::string, public virtual eve::rt::shareable
{

public:
	eve_string(std::string& o) : std::string(o) {};
	eve_string(const std::string& o) : std::string(o) {};
	eve_string(const char* o) : std::string(o) {};
	
	template <class T>
	eve_string(const T& o) : std::string(std::to_string(o)) {};

	eve_string operator + (const eve_string& o)
	{
		return eve_string((std::string&)*this + (std::string&) o);
	}
};

namespace eve
{
namespace io
{

template <>
class write_handler<eve_string> : public eve::event::callback<writable>
{
	eve::io::fd& fd;
	const eve::rt::shared_ptr<const eve_string> buffer;
	ssize_t remaining;
	ssize_t written;

	eve::rt::shared_ptr<eve::rt::function<void, bool> > cb;

public:
	write_handler(eve::io::fd& fd, const eve::rt::shared_ptr<const eve_string>& buffer, const eve::rt::shared_ptr<eve::rt::function<void, bool> >&cb)
		: fd(fd), buffer(buffer), remaining((*buffer).length()), written(0), cb(cb)
	{
	}

	bool operator () (writable& w)
	{
		ssize_t err;
		bool result = false;

		err = ::write(this->fd(), (*this->buffer).c_str() + this->written, this->remaining);

		if (err == this->remaining)
		{
			result = true;
			goto done;

		} else if (err <= 0 && errno != EAGAIN && errno != EWOULDBLOCK)
			goto done;

		/* partial read */
		if (err > 0)
		{
			this->remaining -= err;
			this->written += err;
		}

		/* partial read or EAGAIN */ 
		return true;

done:
		(*this->cb)(result);
		eve::event::off<writable>(this->fd, this);

		return true;
	}
};

}
}