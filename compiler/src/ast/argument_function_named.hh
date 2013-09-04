#ifndef EVE_AST_ARGUMENT_FUNCTION_NAMED_IMPL
#define EVE_AST_ARGUMENT_FUNCTION_NAMED_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

argument_function_named::argument_function_named(const eve::location& location, eve::ast::simple_id* id, eve::ast::expr* expr)
	: eve::ast::argument_function(location), id(id), expr(expr)
{
	this->adopt(id);
	this->adopt(expr);
}

argument_function_named::~argument_function_named()
{
	delete this->id;
	delete this->expr;
}

void argument_function_named::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif