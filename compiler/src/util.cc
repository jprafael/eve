#include "util.hh"

namespace eve
{
namespace util
{

template <class T>
std::vector<T>* make_vector(size_t n, ...)
{
	va_list list;
	va_start(list, n);

	std::vector<T>* v = new std::vector<T>();

	for (size_t i = 0; i < n; i++)
		v->push_back(va_arg(list, T));

	va_end(list);

	return v;
}

std::vector<std::string> list_directory(const std::string& path)
{
	std::vector<std::string> r;
	DIR *dir;
	struct dirent *ent;

	dir = opendir(path.c_str());
	if (!dir)
		return r;

	while ((ent = readdir(dir)) != NULL)
		if (ent->d_name[0] != '.') /* FIXME: ignore directories */
			r.push_back(std::string(ent->d_name));
	
	closedir(dir);

	return r;
}

}
}