#ifndef EVE_LAMBDA_VISITOR_IMPL
#define EVE_LAMBDA_VISITOR_IMPL

#include "visitor.hh"
#include "../driver.hh"

namespace eve
{
namespace visitor
{

lambda_visitor::lambda_visitor(eve::driver& driver)
	: visitor(true), driver(driver), lambda_id(0)
{
}

lambda_visitor::~lambda_visitor()
{
}

void lambda_visitor::handle(eve::ast::expr_lambda* node)
{
	node->id = this->lambda_id++;
}

}
}

#endif