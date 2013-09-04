#ifndef EVE_AST_ARGUMENT_FUNCTION_IMPL
#define EVE_AST_ARGUMENT_FUNCTION_IMPL

namespace eve
{
namespace ast
{

argument_function::argument_function(const eve::location& location)
	: eve::ast::argument(location)
{
}

argument_function::~argument_function()
{
}

void argument_function::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif
