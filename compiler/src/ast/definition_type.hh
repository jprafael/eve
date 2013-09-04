#ifndef EVE_AST_DEFINITION_TYPE_IMPL
#define EVE_AST_DEFINITION_TYPE_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

definition_type::definition_type(const eve::location& location, eve::ast::simple_id* id, std::vector<eve::ast::parameter_type*>* parameters, std::vector<eve::ast::type*>* parents, std::vector<eve::ast::definition*>* definitions)
	: eve::ast::definition(location), id(id), parameters(parameters), parents(parents), definitions(definitions), defaults(0), _context(NULL), generic(NULL)
{
	this->adopt(id);
	this->adopt_all(*parameters);
	this->adopt_all(*parents);
	this->adopt_all(*definitions);
}

definition_type::definition_type(eve::ast::simple_id* id, std::vector<eve::ast::parameter_type*>* parameters)
	: eve::ast::definition(eve::location()), id(id), parameters(parameters), parents(new std::vector<eve::ast::type*>()), definitions(new std::vector<eve::ast::definition*>()), defaults(0), _context(NULL), generic(NULL)
{
	this->adopt(id);
	this->adopt_all(*parameters);
	this->adopt_all(*parents);
	this->adopt_all(*definitions);
}


definition_type::~definition_type()
{
	delete this->id;

	for (size_t i = 0; i < this->parameters->size(); i++)
		delete (*this->parameters)[i];

	delete this->parameters;

	for (size_t i = 0; i < this->parents->size(); i++)
		delete (*this->parents)[i];

	delete this->parents;

	for (size_t i = 0; i < this->definitions->size(); i++)
		delete (*this->definitions)[i];

	delete this->definitions;
}

void definition_type::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& definition_type::context()
{
	return this->_context;
}

bool definition_type::is_generic() const
{
	for (size_t i = 0; i < this->parameters->size(); i++)
		if (dynamic_cast<parameter_type_specialization*>((*this->parameters)[i]) != NULL)
			return false;

	return true;
}

void definition_type::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope)
{
	if (this->builtin)
		return;

	driver.debug("TODO: definition_type::translate()");
}

std::vector<std::string>& definition_type::top_scope(eve::target::cu& cu) const
{
	return cu.type_declarations;
}

eve::ast::type* definition_type::type() const
{
	if (!this->is_generic())
		return this->generic->type();
	
	std::vector<eve::ast::argument_type*>* arguments = new std::vector<eve::ast::argument_type*>();

	for (size_t i = 0; i < this->parameters->size(); i++)
	{
		eve::ast::parameter_type_constant* constant = dynamic_cast<eve::ast::parameter_type_constant*>((*this->parameters)[i]);
		eve::ast::parameter_type_type* type = dynamic_cast<eve::ast::parameter_type_type*>((*this->parameters)[i]);

		std::string id;

		if (constant)
			id = constant->id->string();
		else if (type)
			id = type->id->string();

		eve::ast::argument_type_id* arg = new eve::ast::argument_type_id
		(
			this->location(),
			new eve::ast::simple_id
			(
				this->location(),
				new std::string(id)
			)
		);
		arg->_parent = (*this->parameters)[i]->_parent;

		arguments->push_back(arg);
	}

	eve::ast::type_base* t = new eve::ast::type_base(this->location(), this->id->dup(), arguments);
	t->decl = const_cast<eve::ast::definition_type*>(this);
	t->_parent = const_cast<eve::ast::node*>(static_cast<const eve::ast::node*>(this));

	return t;
}

}
}

#endif
