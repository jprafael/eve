#ifdef EVE_RT_FUNCTION_H

namespace eve
{
namespace rt
{

template <class... Args>
void function_default<Args...>::operator () (Args... args)
{
}

template <class... Args>
const eve::rt::shared_ptr<eve::rt::function<void, Args...> > function_default<Args...>::fn
	= eve::rt::make_shared<eve::rt::function_default<Args...> >();

}
}

#endif