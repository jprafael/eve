#ifndef EVE_UTIL_HH
#define EVE_UTIL_HH

#include <vector>
#include <string>
#include <iostream>

#include <stdarg.h>
#include <dirent.h>

namespace eve
{
namespace util
{

template <class T>
std::vector<T>* make_vector(size_t n, ...);

std::vector<std::string> list_directory(const std::string& path);

}
}

#endif