#ifndef EVE_AST_STMT_RETURN_IMPL
#define EVE_AST_STMT_RETURN_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_return::stmt_return(const eve::location& location, eve::ast::expr* expr)
	: eve::ast::stmt(location), expr(expr)
{
	this->adopt(expr);
}

stmt_return::~stmt_return()
{
	delete this->expr;
}

void stmt_return::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

void stmt_return::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	scope.push_back("return " + this->expr->translate(driver, cu) + ";");
}

}
}

#endif