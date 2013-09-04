#ifndef EVE_AST_DEFINITION_FUNCTION_IMPL
#define EVE_AST_DEFINITION_FUNCTION_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{


definition_function::definition_function(const eve::location& location, eve::ast::type* ret, std::vector<parameter_type*>* template_parameters, std::vector<parameter_function*>* function_parameters, std::vector<perm*>* perms)
	: eve::ast::definition(location), ret(ret), template_parameters(template_parameters), function_parameters(function_parameters), perms(perms), _context(NULL), defaults(0), variadic(false)
{
	if (ret)
		this->adopt(ret);

	this->adopt_all(*template_parameters);
	this->adopt_all(*function_parameters);
	this->adopt_all(*perms);

	if (!function_parameters->empty())
	{
		eve::ast::parameter_function* last = (*function_parameters)[function_parameters->size() - 1];

		if (dynamic_cast<eve::ast::parameter_function_named*>(last) &&
			dynamic_cast<eve::ast::type_elipsis*>(last->type()))
			this->variadic = true;
	}
		
}

definition_function::~definition_function()
{
	delete this->ret;

	for (size_t i = 0; i < this->template_parameters->size(); i++)
		delete (*this->template_parameters)[i];

	delete this->template_parameters;

	for (size_t i = 0; i < this->function_parameters->size(); i++)
		delete (*this->function_parameters)[i];

	delete this->function_parameters;

	for (size_t i = 0; i < this->perms->size(); i++)
		delete (*this->perms)[i];

	delete this->perms;
}

void definition_function::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& definition_function::context()
{
	return this->_context;
}

std::vector<std::string>& definition_function::top_scope(eve::target::cu& cu) const
{
	return cu.function_declarations;
}

}
}

#endif
