#ifndef EVE_AST_DEFINITION_FUNCTION_OPERATOR_IMPL
#define EVE_AST_DEFINITION_FUNCTION_OPERATOR_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

definition_function_operator::definition_function_operator(const eve::location& location, eve::ast::type* type, std::vector<parameter_type*>* template_parameters, std::vector<parameter_function*>* function_parameters, std::vector<perm*>* perms, eve::ast::op op)
	: eve::ast::definition_function(location, type, template_parameters, function_parameters, perms), op(op)
{
}

definition_function_operator::~definition_function_operator()
{
}

void definition_function_operator::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif
