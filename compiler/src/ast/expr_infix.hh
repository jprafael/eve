#ifndef EVE_AST_EXPR_INFIX_IMPL
#define EVE_AST_EXPR_INFIX_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_infix::expr_infix(const eve::location& location, eve::ast::expr* left, eve::ast::expr* right, eve::ast::op op)
	: eve::ast::expr_binary(location, left, right), op(op)
{
}

expr_infix::~expr_infix()
{
}

void expr_infix::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string expr_infix::translate(eve::driver& driver, eve::target::cu& cu)
{
	std::string ret;

	std::string left = this->left->translate(driver, cu);
	std::string right = this->right->translate(driver, cu);

	if (this->op == eve::ast::op::infix_power)
		ret = "pow(" + left + ", " + right + ")";

	else
		ret = "(" + left + eve::ast::to_string(this->op) + right + ")";

	return this->wrap(ret, driver, cu);
}

}
}

#endif
