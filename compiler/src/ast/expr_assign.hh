#ifndef EVE_AST_EXPR_ASSIGN_IMPL
#define EVE_AST_EXPR_ASSIGN_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_assign::expr_assign(const eve::location& location, eve::ast::expr* left, eve::ast::expr* right, eve::ast::op op)
	: eve::ast::expr_binary(location, left, right), op(op)
{
}

expr_assign::~expr_assign()
{
}

void expr_assign::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string expr_assign::translate(eve::driver& driver, eve::target::cu& cu)
{
	std::string ret;

	ret = "(" + this->left->translate(driver, cu) + eve::ast::to_string(this->op) + this->right->translate(driver, cu) + ")";

	return this->wrap(ret, driver, cu);
}


}
}

#endif
