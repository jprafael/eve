#ifndef EVE_AST_PARAMETER_FUNCTION_NAMED_IMPL
#define EVE_AST_PARAMETER_FUNCTION_NAMED_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

parameter_function_named::parameter_function_named(const eve::location& location, eve::ast::type* _type, eve::ast::simple_id* id)
	: eve::ast::parameter_function(location), _type(_type), id(id)
{
	this->adopt(_type);
	this->adopt(id);
}

parameter_function_named::~parameter_function_named()
{
	delete this->_type;
	delete this->id;
}

void parameter_function_named::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::ast::type* parameter_function_named::type() const
{
	return this->_type;
}

}
}

#endif