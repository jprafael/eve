#ifndef EVE_AST_EXPR_ID_IMPL
#define EVE_AST_EXPR_ID_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_id::expr_id(const eve::location& location, eve::ast::id* id)
	: eve::ast::expr(location), id(id)
{
	this->adopt(id);
}

expr_id::~expr_id()
{
	delete this->id;
}

void expr_id::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string expr_id::translate(eve::driver& driver, eve::target::cu& cu)
{
	return this->wrap("eve_" + this->id->string(), driver, cu);
}

}
}

#endif
