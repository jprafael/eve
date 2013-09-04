#ifndef EVE_RT_SHARED_H
#define EVE_RT_SHARED_H

#include <type_traits>
#include "monitor.hpp"

namespace eve
{
namespace rt
{

class shareable
{
public:
	shareable() {};
	virtual ~shareable() {};
};

template <typename T>
class shared_wrapper : public virtual shareable
{
	T object;

public:
	template <typename... A>
	shared_wrapper(A&&... args);

	virtual ~shared_wrapper();

	operator T*();
	operator const T*() const;
};

template <class T>
class shared_ptr;

class shared_control
{

public:
	explicit shared_control(shareable* object);
	~shared_control();

	eve::rt::monitor _monitor;
	volatile int refs;
	shareable* _object;
	
	void incr();
	void decr();
};

template <class T>
class shared_ptr
{
	
public:
	/* FIXME: this should be protected, and shared_ptr<B> should be a friend */
	shared_control* _control;

	/* default constructor (NULL pointer) */
	virtual ~shared_ptr();

	/* used by make_shared and default constructor */
	explicit shared_ptr(shared_control* control = NULL);

	/* copy constructor */
	shared_ptr(const shared_ptr<T>& other);

	template <typename B>
	shared_ptr(const shared_ptr<B>& other);

	/* move constructor */
	shared_ptr(shared_ptr<T>&& other);

	template <typename B>
	shared_ptr(shared_ptr<B>&& other);

	/* copy assign */
	shared_ptr<T>& operator = (const shared_ptr<T>& other);

	template <typename B>
	shared_ptr<T>& operator = (const shared_ptr<B>& other);

	/* move assign */
	shared_ptr<T>& operator = (shared_ptr<T>&& other);

	template <typename B>
	shared_ptr<T>& operator = (shared_ptr<B>&& other);

	/* used for equality tests (inside sets and such) */
	bool operator == (const shared_ptr<T>& other) const;
	bool operator < (const shared_ptr<T>& other) const;

	operator bool () const;

	eve::rt::monitor& monitor();
	const eve::rt::monitor& monitor() const;

	/* cast operators to normal pointers */
	operator T* ();
	operator const T* () const;

	/* they use these functions */
	template<class B>
	B* to(typename std::enable_if<std::is_base_of<eve::rt::shareable, B>::value, void*>::type = 0);

	template<class B>
	const B* to(typename std::enable_if<std::is_base_of<eve::rt::shareable, B>::value, void*>::type = 0) const;

	template<class B>
	B* to(typename std::enable_if<not std::is_base_of<eve::rt::shareable, B>::value, void*>::type = 0);

	template<class B>
	const B* to(typename std::enable_if<not std::is_base_of<eve::rt::shareable, B>::value, void*>::type = 0) const;

	inline void prevent_gc();
	inline void allow_gc();
};

template <class T, typename... A>
eve::rt::shared_ptr<T>
make_shared(A&&... args);

}
}

#endif