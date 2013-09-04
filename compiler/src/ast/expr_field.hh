#ifndef EVE_AST_EXPR_FIELD_IMPL
#define EVE_AST_EXPR_FIELD_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_field::expr_field(const eve::location& location, eve::ast::expr* expr, eve::ast::simple_id* id)
	: eve::ast::expr(location), expr(expr), id(id)
{
	this->adopt(expr);
	this->adopt(id);
}

expr_field::~expr_field()
{
	delete this->expr;
	delete this->id;
}

void expr_field::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string expr_field::translate(eve::driver& driver, eve::target::cu& cu)
{
	std::string ret;

	ret += this->expr->translate(driver, cu);

	if (this->expr->type()->base()->is("static"))
		ret += "::";

	else
		ret += ".";

	ret += this->id->string();

	return this->wrap(ret, driver, cu);
}

}
}

#endif
