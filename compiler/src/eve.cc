#include <iostream>
#include <fstream>

#include <stdio.h>
#include <unistd.h>

#include "driver.hh"
#include "ast/ast.hh"

char buffer[FILENAME_MAX];

int main(int argc, char *argv[])
{
	char* path = getcwd(buffer, FILENAME_MAX);
	if (path != buffer)
	{
		std::cerr << "failed to get current working directory" << std::endl;
		return -1;
	}

	std::string current_path = std::string(buffer);
	eve::driver driver(current_path);

	for (int ai = 1; ai < argc; ++ai)
	{
		if (argv[ai] == std::string ("-p"))
			driver.trace_parsing(true);

		else if (argv[ai] == std::string ("-s"))
			driver.trace_scanning(true);

		driver.require(std::string(argv[ai]));
	}

	driver.parse();

	if (driver.errors())
		return 1;

	driver.analyse();

	if (driver.errors())
		return 1;

	driver.generate();

	return 0;
}
