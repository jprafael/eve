#ifndef EVE_AST_PARAMETER_TYPE_TYPE_IMPL
#define EVE_AST_PARAMETER_TYPE_TYPE_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

parameter_type_type::parameter_type_type(const eve::location& location, eve::ast::simple_id* id, eve::ast::type* type)
	: eve::ast::parameter_type(location), id(id), type(type), elipsis(false)
{
	this->adopt(this->id);

	if (this->type)
		this->adopt(this->type);
}

parameter_type_type::parameter_type_type(const eve::location& location, eve::ast::simple_id* id)
	: eve::ast::parameter_type(location), id(id), type(NULL), elipsis(true)
{
	this->adopt(this->id);
}

parameter_type_type::~parameter_type_type()
{
	delete this->id;

	if (this->type)
		delete this->type;
}

void parameter_type_type::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif