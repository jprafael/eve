#ifndef EVE_RT_TASK_H
#define EVE_RT_TASK_H

#include "platform.hpp"

namespace eve
{
namespace rt
{

class task
{

public:
	task();
	virtual ~task();

	virtual void operator () () = 0;
};

}
}

#endif
