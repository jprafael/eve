#ifndef EVE_AST_PARAMETER_FUNCTION_IMPL
#define EVE_AST_PARAMETER_FUNCTION_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

parameter_function::parameter_function(const eve::location& location)
	: eve::ast::parameter(location)
{
}

parameter_function::~parameter_function()
{
}

void parameter_function::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif