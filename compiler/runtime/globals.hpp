eve::rt::shared_ptr<eve::io::fd> eve_stdout = eve::rt::make_shared<eve::io::fd>(fileno(stdout));
eve::rt::shared_ptr<eve::io::fd> eve_stdin = eve::rt::make_shared<eve::io::fd>(fileno(stdin));