#ifdef EVE_RT_SHARED_H

#include <sched.h>
#include <iostream>
#include <type_traits>

#include <cassert>

namespace eve
{
namespace rt
{

/*
	shared_wrapper<T>
	
	allows the creation of a shared_ptr<T> when T is a native
	it works by wrapping T inside a shared_wrapper<T>.
	shared_wrapper<T> then has a cast operator to T returning the reference back,
	allowing the use of normal operators
*/
template <typename T>
template <typename... A>
shared_wrapper<T>::shared_wrapper(A&&... args)
	: object(std::forward<A&&>(args)...)
{
}

template <typename T>
shared_wrapper<T>::~shared_wrapper()
{
}

template <typename T>
shared_wrapper<T>::operator T* ()
{
	return &this->object;
}

template <typename T>
shared_wrapper<T>::operator const T* () const
{
	return &this->object;
}

/*
	shared_control
	
	manager block of shared_ptr, contains:
		The pointer to the object
		The number of active references
		A monitor to synchronize accesses to object
*/

shared_control::shared_control(shareable* object)
	: refs(1), _object(object)
{
}

shared_control::~shared_control()
{
	delete this->_object;
}

void shared_control::incr()
{
	__sync_add_and_fetch(&this->refs, 1);
}

void shared_control::decr()
{
	if (__sync_sub_and_fetch(&this->refs, 1) == 0)
		delete this;
}

/*
	shared_ptr
	the object containing a single pointer to the control block
*/
template <typename T>
shared_ptr<T>::shared_ptr(shared_control* control)
	: _control(control)
{
}

template <typename T>
shared_ptr<T>::~shared_ptr()
{
	if (this->_control)
		this->_control->decr();
}

template <typename T>
shared_ptr<T>::shared_ptr(shared_ptr<T>&& other)
	: _control(other._control)
{
	assert(other._control);
	other._control = NULL;
}

template <typename T>
template <typename B>
shared_ptr<T>::shared_ptr(shared_ptr<B>&& other)
	: _control(other._control)
{
	assert(other._control);
	static_assert
	(
		std::is_convertible<B, T>::value || std::is_base_of<T, B>::value,
		"move constructor of shared_ptr of non convertible types"
	);

	other._control = NULL;
}

template <typename T>
shared_ptr<T>::shared_ptr(const shared_ptr<T>& other)
	: _control(other._control)
{
	assert(other._control);
	this->_control->incr();
}


template <typename T>
template <typename B>
shared_ptr<T>::shared_ptr(const shared_ptr<B>& other)
	: _control(other._control)
{
	assert(other._control);

	static_assert
	(
		std::is_convertible<B, T>::value || std::is_base_of<T, B>::value,
		"move constructor of shared_ptr of non convertible types"
	);

	this->_control->incr();
}

template <typename T>
shared_ptr<T>& shared_ptr<T>::operator = (const shared_ptr<T>& other)
{
	assert(other._control);

	if (this != &other && this->_control != other._control)
	{
		if (this->_control)
			this->_control->decr();

		this->_control = other._control;
		this->_control->incr();
	}

	return *this;
}

template <typename T>
template <typename B>
shared_ptr<T>& shared_ptr<T>::operator = (const shared_ptr<B>& other)
{
	assert(other._control);

	static_assert
	(
		std::is_convertible<B, T>::value || std::is_base_of<T, B>::value,
		"copy assignment between shared_ptr of non convertible types"
	);

	if (this != &other && this->_control != other._control)
	{
		if (this->_control)
			this->_control->decr();

		this->_control = other._control;
		this->_control->incr();
	}

	return *this;
}

template <typename T>
shared_ptr<T>& shared_ptr<T>::operator = (shared_ptr<T>&& other)
{
	assert(other._control);

	if (this->_control)
		this->_control->decr();

	this->_control = other._control;
	other._control = NULL;

	return *this;
}

template <typename T>
bool shared_ptr<T>::operator == (const shared_ptr<T>& other) const 
{
	return this->_control == other._control;
}

template <typename T>
bool shared_ptr<T>::operator < (const shared_ptr<T>& other) const 
{
	return this->_control < other._control;
}

template <typename T>
eve::rt::monitor& shared_ptr<T>::monitor()
{
	assert (this->_control);
	return this->_control->_monitor;
}


template <typename T>
const eve::rt::monitor& shared_ptr<T>::monitor() const
{
	assert(this->_control);
	return this->_control->_monitor;
}

template <typename T>
shared_ptr<T>::operator bool () const
{
	return this->_control;
}

template <typename T>
shared_ptr<T>::operator T* ()
{
	return this->to<T>();
}

template <typename T>
shared_ptr<T>::operator const T* () const
{
	return this->to<T>();
}

template <typename T>
template <class B>
B* shared_ptr<T>::to(typename std::enable_if<std::is_base_of<eve::rt::shareable, B>::value, void*>::type)
{
	B* ptr;

	if (!this->_control)
		ptr = NULL;

 	else
 	{
		ptr = dynamic_cast<B*>(this->_control->_object);
		assert(ptr);
	}

	return ptr;
}

template <typename T>
template <class B>
const B* shared_ptr<T>::to(typename std::enable_if<std::is_base_of<eve::rt::shareable, B>::value, void*>::type) const 
{
	B* ptr;

	if (!this->_control)
		ptr = NULL;

 	else
 	{
		ptr = dynamic_cast<B*>(this->_control->_object);
		assert(ptr);
	}

	return ptr;
}

template <typename T>
template <class B>
B* shared_ptr<T>::to(typename std::enable_if<not std::is_base_of<eve::rt::shareable, B>::value, void*>::type)
{
	B* ptr;

	if (!this->_control)
		ptr = NULL;

	else
	{
		ptr = (T*) *dynamic_cast<eve::rt::shared_wrapper<T>*>(this->_control->_object);
		assert(ptr);
	}

	return ptr;
}

template <typename T>
template <class B>
const B* shared_ptr<T>::to(typename std::enable_if<not std::is_base_of<eve::rt::shareable, B>::value, void*>::type) const 
{
	B* ptr;

	if (!this->_control)
		ptr = NULL;

 	else
 	{
		ptr = (T*) *dynamic_cast<eve::rt::shared_wrapper<T>*>(this->_control->_object);
		assert(ptr);
	}

	return ptr;
}

template <typename T>
void shared_ptr<T>::prevent_gc()
{
	this->_control->incr();
}

template <typename T>
void shared_ptr<T>::allow_gc()
{
	this->_control->decr();
}

template <typename T, typename... A>
eve::rt::shared_ptr<T>
make_shared(A&&... args)
{
	typedef typename std::conditional<
		std::is_base_of<eve::rt::shareable, T>::value,
		T,
		eve::rt::shared_wrapper<T>
	>::type K;

	return shared_ptr<T>(new shared_control(new K(std::forward<A&&>(args)...)));
}

}
}

#endif