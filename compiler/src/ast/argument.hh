#ifndef EVE_AST_ARGUMENT_IMPL
#define EVE_AST_ARGUMENT_IMPL

namespace eve
{
namespace ast
{

argument::argument(const eve::location& location)
	: eve::ast::node(location)
{
}

argument::~argument()
{
}

void argument::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif
