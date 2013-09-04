#ifndef EVE_EVENT_EMITTER_H
#define EVE_EVENT_EMITTER_H

#include <memory>
#include <vector>

#include "../rt/function.hpp"

namespace eve
{
namespace event
{

template <class E>
using callback = eve::rt::function<bool, E&>; 

template <class E>
struct null_callback : public callback<E>
{
public:
	bool operator () (E& e) { return false; }
};

template <class E>
class emitter
{
	std::vector<eve::rt::shared_ptr<callback<E> > > cbs;

public:
	emitter();

	bool has_callback() const;

	void emit(E& args);
	void on(const eve::rt::shared_ptr<callback<E> >& cb);

	void off(callback<E>* cb);
	void off();
};

/* helper functions to bind template arguments (avoids having to use static_cast)*/
template <class E>
bool has_callback(const emitter<E>& object);

template <class E>
void emit(emitter<E>& object, E& e);

template <class E>
void on(emitter<E>& object, const eve::rt::shared_ptr<callback<E> >& cb);

template <class E>
void off(emitter<E>& object, callback<E>* cb);

template <class E>
void off(emitter<E>& object);

}
}
#endif