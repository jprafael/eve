#ifndef EVE_AST_DEFINITION_FUNCTION_OPERATOR_EXPLICIT_IMPL
#define EVE_AST_DEFINITION_FUNCTION_OPERATOR_EXPLICIT_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

definition_function_operator_explicit::definition_function_operator_explicit(const eve::location& location, eve::ast::type* type, std::vector<parameter_type*>* template_parameters, std::vector<parameter_function*>* function_parameters, std::vector<perm*>* perms, eve::ast::op op, std::vector<stmt*>* stmts)
	: eve::ast::definition_function_operator(location, type, template_parameters, function_parameters, perms, op), stmts(stmts)
{
	this->adopt_all(*this->stmts);
}

definition_function_operator_explicit::~definition_function_operator_explicit()
{
	for (size_t i = 0; i < this->stmts->size(); i++)
		delete (*this->stmts)[i];

	delete this->stmts;
}

void definition_function_operator_explicit::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

void definition_function_operator_explicit::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope)
{
	if (this->builtin)
		return;

	driver.debug("TODO: definition_function_operator_explicit::translate()");
}

}
}

#endif
