#ifndef EVE_RT_FUNCTION_H
#define EVE_RT_FUNCTION_H

#include <memory>

#include "shared.hpp"

namespace eve
{
namespace rt
{

template <class R, class... Args>
class function : public virtual eve::rt::shareable
{

public:
	function() { };
	virtual ~function() { };

	virtual R operator() (Args... args) = 0;
};

template <class... Args>
class function_default : public function<void, Args...>
{

public:
	static const eve::rt::shared_ptr<eve::rt::function<void, Args...> > fn;

	virtual void operator() (Args... args);
};

}
}

#endif