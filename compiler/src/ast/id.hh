#ifndef EVE_AST_ID_IMPL
#define EVE_AST_ID_IMPL

namespace eve
{
namespace ast
{

id::id(const eve::location& location)
	: eve::ast::node(location)
{
}

id::~id()
{
}

void id::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif