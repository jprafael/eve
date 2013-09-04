#ifdef EVE_EVENT_EMITTER_H

namespace eve
{
namespace event
{

template <class E>
emitter<E>::emitter()
{
}

template <class E>
bool emitter<E>::has_callback() const
{
	return !this->cbs.empty();
}

template <class E>
void emitter<E>::on(const eve::rt::shared_ptr<callback<E> >& cb)
{
	this->cbs.push_back(cb);
}

template <class E>
void emitter<E>::off(callback<E>* cb)
{
	typename std::vector<eve::rt::shared_ptr<callback<E> > >::iterator it = this->cbs.begin(), end = this->cbs.end();

	for (; it != end; ++it)
	{
		if (&(**it) == cb)
		{
			this->cbs.erase(it);
			return;
		}
	}

	assert(false);
}

template <class E>
void emitter<E>::off()
{
	this->cbs.clear();
}

template <class E>
bool has_callback(const emitter<E>& object)
{
	return object.has_callback();
}

template <class E>
void emitter<E>::emit(E& e)
{
	for (size_t i = 0; i < this->cbs.size(); i++)
		if ((*this->cbs[i])(e))
			return;
}

template <class E>
void emit(emitter<E>& object, E& e)
{
	object.emit(e);
}

template <class E>
void on(emitter<E>& object, const eve::rt::shared_ptr<callback<E> >& cb)
{
	object.on(cb);
}

template <class E>
void off(emitter<E>& object, callback<E>* cb)
{
	object.off(cb);
}

template <class E>
void off(emitter<E>& object)
{
	object.off();
}

}
};

#endif