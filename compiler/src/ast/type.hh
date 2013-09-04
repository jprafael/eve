#ifndef EVE_AST_TYPE_IMPL
#define EVE_AST_TYPE_IMPL

namespace eve
{
namespace ast
{

type::type(const eve::location& location)
	: eve::ast::node(location)
{
}

type::~type()
{
}

void type::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif