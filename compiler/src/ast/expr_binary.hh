#ifndef EVE_AST_EXPR_BINARY_IMPL
#define EVE_AST_EXPR_BINARY_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_binary::expr_binary(const eve::location& location, eve::ast::expr* left, eve::ast::expr* right)
	: eve::ast::expr(location), left(left), right(right)
{
	this->adopt(left);
	this->adopt(right);
}

expr_binary::~expr_binary()
{
	delete this->left;
	delete this->right;
}

void expr_binary::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif
