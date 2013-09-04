#ifndef EVE_AST_EXPR_UNARY_IMPL
#define EVE_AST_EXPR_UNARY_IMPL

namespace eve
{
namespace ast
{

expr_unary::expr_unary(const eve::location& location, eve::ast::expr* expr)
	: eve::ast::expr(location), expr(expr)
{
	this->adopt(expr);
}

expr_unary::~expr_unary()
{
	delete this->expr;
}

void expr_unary::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif
