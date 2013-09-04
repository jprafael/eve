#ifndef EVE_AST_EXPR_INCR_IMPL
#define EVE_AST_EXPR_INCR_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_incr::expr_incr(const eve::location& location, eve::ast::expr* expr, eve::ast::op op, bool prefix)
	: eve::ast::expr_unary(location, expr), op(op), prefix(prefix)
{
}

expr_incr::~expr_incr()
{
}

void expr_incr::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string expr_incr::translate(eve::driver& driver, eve::target::cu& cu)
{
	std::string ret;

	if (this->prefix)
		ret = "(" + eve::ast::to_string(this->op) + this->expr->translate(driver, cu) + ")";

	else
		ret = "(" + this->expr->translate(driver, cu) + eve::ast::to_string(this->op) + ")";

	return this->wrap(ret, driver, cu);
}

}
}

#endif
