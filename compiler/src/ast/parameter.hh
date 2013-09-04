#ifndef EVE_AST_PARAMETER_IMPL
#define EVE_AST_PARAMETER_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

parameter::parameter(const eve::location& location)
	: eve::ast::node(location)
{
}

parameter::~parameter()
{
}

void parameter::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif