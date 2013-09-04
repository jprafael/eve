#ifndef EVE_AST_PARAMETER_TYPE_CONSTANT_IMPL
#define EVE_AST_PARAMETER_TYPE_CONSTANT_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

parameter_type_constant::parameter_type_constant(const eve::location& location, eve::ast::simple_id* id, eve::ast::constant* constant)
	: eve::ast::parameter_type(location), id(id), constant(constant), elipsis(false)
{
	this->adopt(this->id);
	this->adopt(this->constant);
}

parameter_type_constant::parameter_type_constant(const eve::location& location, eve::ast::simple_id* id)
	: eve::ast::parameter_type(location), id(id), constant(NULL), elipsis(true)
{
	this->adopt(this->id);
}

parameter_type_constant::~parameter_type_constant()
{
	delete this->id;

	if (this->constant)
		delete this->constant;
}

void parameter_type_constant::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif