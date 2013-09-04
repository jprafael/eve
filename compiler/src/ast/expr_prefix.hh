#ifndef EVE_AST_EXPR_PREFIX_IMPL
#define EVE_AST_EXPR_PREFIX_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_prefix::expr_prefix(const eve::location& location, eve::ast::expr* expr, eve::ast::op op)
	: eve::ast::expr_unary(location, expr), op(op)
{
}

expr_prefix::~expr_prefix()
{
}

void expr_prefix::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string expr_prefix::translate(eve::driver& driver, eve::target::cu& cu)
{
	std::string ret = eve::ast::to_string(this->op) + this->expr->translate(driver, cu);

	return this->wrap(ret, driver, cu);
}

}
}

#endif
