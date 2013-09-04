#ifndef EVE_AST_STMT_ON_IMPL
#define EVE_AST_STMT_ON_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_on::stmt_on(const eve::location& location, eve::ast::expr* expr, eve::ast::simple_id* id, eve::ast::expr* lambda)
	: eve::ast::stmt(location), expr(expr), id(id), lambda(lambda)
{
	this->adopt(expr);
	this->adopt(id);
	this->adopt(lambda);
}

stmt_on::~stmt_on()
{
	delete this->expr;
	delete this->id;
	delete this->lambda;
}

void stmt_on::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

void stmt_on::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	std::string str = std::string("eve::event::on<eve_") + this->id->string() + ">(";

	str += this->expr->translate(driver, cu);
	str += ",";
	str += this->lambda->translate(driver, cu);
	str += ");";

	scope.push_back(str);
}

}
}
#endif
