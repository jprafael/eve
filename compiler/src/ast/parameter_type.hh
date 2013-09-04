#ifndef EVE_AST_PARAMETER_TYPE_IMPL
#define EVE_AST_PARAMETER_TYPE_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

parameter_type::parameter_type(const eve::location& location)
	: eve::ast::parameter(location)
{
}

parameter_type::~parameter_type()
{
}

void parameter_type::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif