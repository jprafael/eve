#ifndef EVE_AST_ARGUMENT_TYPE_ID_IMPL
#define EVE_AST_ARGUMENT_TYPE_ID_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

argument_type_id::argument_type_id(const eve::location& location, eve::ast::id* id)
	: eve::ast::argument_type(location), id(id)
{
	this->adopt(id);
}

argument_type_id::~argument_type_id()
{
	delete this->id;
}

void argument_type_id::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string argument_type_id::translate(eve::driver& driver, eve::target::cu& cu)
{
	return std::string("eve_") + this->id->string();
}

argument_type_id* argument_type_id::dup() const
{
	eve::ast::argument_type_id* id = new argument_type_id(this->location(), this->id->dup());
	id->_parent = this->_parent;

	return id;
}

bool argument_type_id::equals(const eve::ast::argument_type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const
{
	if (this == rhs)
		return true;

	const eve::ast::argument_type_id* id = dynamic_cast<const eve::ast::argument_type_id*>(rhs);
	const eve::ast::argument_type_constant* constant = dynamic_cast<const eve::ast::argument_type_constant*>(rhs);
	const eve::ast::argument_type_type* type = dynamic_cast<const eve::ast::argument_type_type*>(rhs);
	
	if (id)
		return this->id->string() == id->id->string();

	if (constant)
	{
		out_bindings[this->id->string()] = rhs->dup();
		return true;
	}

	if (type)
	{
		std::vector<eve::ast::node*> decl = this->context().lookup(this->id);

		eve::ast::parameter_type_type* param_type = dynamic_cast<eve::ast::parameter_type_type*>(decl[0]);
		if (!param_type)
		{
			eve::ast::definition_type* def_type = dynamic_cast<eve::ast::definition_type*>(decl[0]);
			assert(def_type);

			return def_type->type()->equals(type->type, out_bindings);
		}

		out_bindings[this->id->string()] = rhs->dup();
		return true;

	}

	assert(false);
}

argument_type* argument_type_id::bind(std::map<std::string, eve::ast::argument_type*> bindings) const
{
	std::map<std::string, eve::ast::argument_type*>::iterator it;
	it = bindings.find(this->id->string());

	if (it != bindings.end())
		return it->second;

	return this->dup();
}

}
}

#endif