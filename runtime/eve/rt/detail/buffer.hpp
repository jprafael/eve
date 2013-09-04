#ifdef EVE_RT_BUFFER_H

namespace eve
{
namespace rt
{

template <typename T>
buffer<T>::buffer(unsigned int log_initial_capacity)
	: capacity(1 << log_initial_capacity), mask(capacity - 1), top(0), bottom(0)
{
	this->data = new T[this->capacity];
}

template <typename T>
buffer<T>::~buffer()
{
	while (!this->pages.empty())
	{
		delete [] this->pages.top();
		this->pages.pop();
	}

	/* current data is not in the pages stack */
	delete [] this->data;
}

template <typename T>
inline void buffer<T>::push_top(const T& value)
{
	this->data[this->top & this->mask] = value;

	this->top++;
	this->maybe_grow();
}

template <typename T>
inline T buffer<T>::pop_top()
{
	assert(!this->empty());

	this->top--;
	this->maybe_shrink();

	return this->data[this->top & this->mask];
}

template <typename T>
inline T buffer<T>::pop_bottom()
{
	assert(!this->empty());

	T value = this->data[this->bottom & this->mask];

	this->bottom++;
	return value;
}

template <typename T>
inline uint64_t buffer<T>::size() const
{
	return this->top - this->bottom;
}

template <typename T>
inline bool buffer<T>::empty() const
{
	return this->bottom == this->top;
}

template <typename T>
inline bool buffer<T>::full() const
{
	return this->size() == this->capacity;
}

template <typename T>
inline void buffer<T>::maybe_grow()
{
	if (unlikely(this->full()))
	{
		uint64_t old_mask = this->mask;
		T* old_data = this->data;

		this->pages.push(this->data);

		this->capacity <<= 1;
		this->mask = this->capacity - 1;

		T* new_data = new T[this->capacity];
		for (uint64_t i = this->bottom; i < this->top; i++)
			new_data[i & this->mask] = old_data[i & old_mask];

		/* only change data here, otherwise a thread might interrupt grow()/shrink() while new_data is not correct yet */
		this->data = new_data;
	}
}

template <typename T>
inline void buffer<T>::maybe_shrink()
{
	/* FIXME: this somehow causes a bug consistently */
	return;

	/* removes pages 2 by 2 to avoid thrashing */
	uint64_t size = this->size();

	if (unlikely(this->pages.size() > 2 && size < (this->capacity >> 2)))
	{
		T* old_data = this->data;
		uint64_t old_mask = this->mask;

		this->capacity >>= 2;
		this->mask = this->capacity - 1;

		delete [] this->pages.top();
		this->pages.pop();

		T* new_data = this->pages.top();
		this->pages.pop();

		for (uint64_t i = this->bottom; i < this->top; i++)
			this->data[i & this->mask] = old_data[i & old_mask];

		this->data = new_data;
		delete [] old_data;
	}
}

}
}

#endif