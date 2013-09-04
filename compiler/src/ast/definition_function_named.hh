#ifndef EVE_AST_DEFINITION_FUNCTION_NAMED_IMPL
#define EVE_AST_DEFINITION_FUNCTION_NAMED_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{


definition_function_named::definition_function_named(const eve::location& location, eve::ast::type* type, std::vector<parameter_type*>* template_parameters, std::vector<parameter_function*>* function_parameters, std::vector<perm*>* perms, eve::ast::simple_id* id)
	: eve::ast::definition_function(location, type, template_parameters, function_parameters, perms), id(id)
{
	this->adopt(id);
}

definition_function_named::~definition_function_named()
{
	delete this->id;
}

void definition_function_named::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif
