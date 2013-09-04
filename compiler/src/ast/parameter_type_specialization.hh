#ifndef EVE_AST_PARAMETER_TYPE_SPECIALIZATION_IMPL
#define EVE_AST_PARAMETER_TYPE_SPECIALIZATION_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

parameter_type_specialization::parameter_type_specialization(const eve::location& location, eve::ast::argument_type* argument)
	: eve::ast::parameter_type(location)
{
	this->adopt(argument);
}

parameter_type_specialization::~parameter_type_specialization()
{
	delete this->argument;
}

void parameter_type_specialization::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif