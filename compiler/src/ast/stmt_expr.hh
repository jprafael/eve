#ifndef EVE_AST_STMT_EXPR_IMPL
#define EVE_AST_STMT_EXPR_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_expr::stmt_expr(const eve::location& location, eve::ast::expr* expr)
	: eve::ast::stmt(location), expr(expr)
{
	this->adopt(expr);
}

stmt_expr::~stmt_expr()
{
	delete this->expr;
}

void stmt_expr::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

void stmt_expr::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	scope.push_back(this->expr->translate(driver, cu) + ";");
}

}
}

#endif