#ifndef EVE_AST_ARGUMENT_FUNCTION_EXPR_IMPL
#define EVE_AST_ARGUMENT_FUNCTION_EXPR_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

argument_function_expr::argument_function_expr(const eve::location& location, eve::ast::expr* expr)
	: eve::ast::argument_function(location), expr(expr)
{
	this->adopt(expr);
}

argument_function_expr::~argument_function_expr()
{
	delete this->expr;
}

void argument_function_expr::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif