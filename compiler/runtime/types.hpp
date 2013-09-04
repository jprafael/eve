#include <set>

#include "string.hpp"

typedef void eve_void;
typedef char eve_char;
typedef bool eve_bool;

template <int S = 32>
struct eve_int_helper
{
	typedef int type;
};

template <>
struct eve_int_helper<8>
{
	typedef int8_t type;
};

template <>
struct eve_int_helper<16>
{
	typedef int16_t type;
};

template <>
struct eve_int_helper<64>
{
	typedef int64_t type;
};

template <int S = 32>
using eve_int = typename eve_int_helper<S>::type;

template <int S = 32>
struct eve_float_helper
{
	typedef float type;
};

template <>
struct eve_float_helper<64>
{
	typedef double type;
};

template <int S = 32>
using eve_float = typename eve_float_helper<S>::type;

typedef eve::io::fd eve_fd;
typedef eve::io::socket eve_socket;
typedef eve::io::server_socket eve_server_socket;
typedef eve::io::listening eve_listening;
typedef eve::io::connection eve_connection;
typedef eve::io::readable eve_readable;
typedef eve::io::closed eve_closed;
typedef eve::util::timer eve_timer;

/* misc */
template <typename T>
using eve_vector = std::vector<T>;

template <typename T>
using eve_set = std::set<T>;

template <typename T>
using eve_reference = typename std::add_lvalue_reference<T>::type;

template <typename T>
using eve_iterator = typename std::set<T>::iterator;


template <typename T, typename... A>
using eve_function = eve::rt::function<T, A...>;

/* shared types*/
template <typename T>
using eve_shared = eve::rt::shared_ptr<T>;