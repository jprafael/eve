#ifndef EVE_AST_ARGUMENT_TYPE_IMPL
#define EVE_AST_ARGUMENT_TYPE_IMPL

namespace eve
{
namespace ast
{

argument_type::argument_type(const eve::location& location)
	: eve::ast::argument(location)
{
}

argument_type::~argument_type()
{
}

void argument_type::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif