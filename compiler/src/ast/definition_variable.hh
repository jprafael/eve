#ifndef EVE_AST_DEFINITION_VARIABLE_IMPL
#define EVE_AST_DEFINITION_VARIABLE_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

definition_variable::definition_variable(const eve::location& location, eve::ast::type* type, eve::ast::simple_id* id, eve::ast::expr* argument)
	: eve::ast::definition(location), type(type), id(id)
{
	this->adopt(type);
	this->adopt(id);
	this->adopt(argument);

	this->arguments = new std::vector<eve::ast::expr*>();
	this->arguments->push_back(argument);

	this->named_arguments = new std::vector<eve::ast::argument_function_named*>();
}

definition_variable::definition_variable(const eve::location& location, eve::ast::type* type, eve::ast::simple_id* id, std::vector<eve::ast::expr*>* arguments, std::vector<eve::ast::argument_function_named*>* named_arguments)
	: eve::ast::definition(location), type(type), id(id), arguments(arguments), named_arguments(named_arguments)
{
	this->adopt(type);
	this->adopt(id);
	this->adopt_all(*arguments);
	this->adopt_all(*named_arguments);
}

definition_variable::~definition_variable()
{
	delete this->type;
	delete this->id;

	for (size_t i = 0; i < this->arguments->size(); i++)
		delete (*this->arguments)[i];

	delete this->arguments;

	for (size_t i = 0; i < this->named_arguments->size(); i++)
		delete (*this->named_arguments)[i];

	delete this->named_arguments;
}

void definition_variable::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

void definition_variable::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope)
{
	if (this->builtin)
		return;

	std::string str = this->type->translate(driver, cu) + " eve_" + this->id->string();

	bool up_cast = this->type->is("shared");
	if (this->arguments->size() == 1 && (*this->arguments)[0]->type()->is("shared"))
		up_cast = false;

	if (up_cast)
	{
		eve::ast::argument_type* base = this->type->bindings()["T"];

		str += std::string(" = eve::rt::make_shared<") + base->translate(driver, cu) + ">";

		if (this->arguments->size() == 0)
			str += "()";
	}

	size_t total = this->arguments->size() + this->named_arguments->size();

	if (total > 0)
	{
		str += "(";

		for (size_t i = 0; i < total; i++)
		{
			eve::ast::expr* arg;

			if (i < this->arguments->size())
				arg = (*this->arguments)[i];
			else
				arg = (*this->named_arguments)[i - this->arguments->size()]->expr;

			str += arg->translate(driver, cu);

			if (i != total - 1)
				str += ", ";
		}

		str += ")";
	}

	str += ";";

	scope.push_back(str);
}

std::vector<std::string>& definition_variable::top_scope(eve::target::cu& cu) const
{
	return cu.variable_declarations;
}

}
}

#endif
