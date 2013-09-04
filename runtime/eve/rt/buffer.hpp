#ifndef EVE_RT_BUFFER_H
#define EVE_RT_BUFFER_H

#include <stack>

namespace eve
{
namespace rt
{

template <typename T>
class buffer
{
	T* data;

	uint64_t capacity;
	uint64_t mask;

	uint64_t top;
	uint64_t bottom;

	std::stack<T*> pages;

private:
	inline void maybe_grow();
	inline void maybe_shrink();

public:
	buffer(unsigned int log_size = 7);
	~buffer();

	/* called by the owner thread*/
	inline void push_top(const T& value);
	inline T pop_top();

	/* called by stealers, that already stopped this thread */
	inline T pop_bottom();

	inline uint64_t size() const;
	inline bool empty() const;
	inline bool full() const;
};

}
}

#endif