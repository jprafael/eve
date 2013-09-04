void __init(eve::rt::runtime& rt)
{
	(*eve_stdout).set_shared(eve_stdout);
	(*eve_stdin).set_shared(eve_stdin);

	rt.epoll().add(eve::io::epoll::close, (*eve_stdout), &(*eve_stdout).handler());
	rt.epoll().add(eve::io::epoll::close, (*eve_stdin), &(*eve_stdout).handler());
}