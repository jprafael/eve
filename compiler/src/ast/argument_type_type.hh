#ifndef EVE_AST_ARGUMENT_TYPE_TYPE_IMPL
#define EVE_AST_ARGUMENT_TYPE_TYPE_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

argument_type_type::argument_type_type(const eve::location& location, eve::ast::type* type)
	: eve::ast::argument_type(location), type(type)
{
	this->adopt(type);
}

argument_type_type::~argument_type_type()
{
	delete this->type;
}

void argument_type_type::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string argument_type_type::translate(eve::driver& driver, eve::target::cu& cu)
{
	return this->type->translate(driver, cu);
}

argument_type_type* argument_type_type::dup() const
{
	eve::ast::argument_type_type* type = new argument_type_type(this->location(), this->type->dup());
	type->_parent = this->_parent;
	return type;
}

bool argument_type_type::equals(const eve::ast::argument_type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const
{
	if (this == rhs)
		return true;

	const eve::ast::argument_type_id* id = dynamic_cast<const eve::ast::argument_type_id*>(rhs);
	const eve::ast::argument_type_type* base = dynamic_cast<const eve::ast::argument_type_type*>(rhs);
	const eve::ast::argument_type_constant* constant = dynamic_cast<const eve::ast::argument_type_constant*>(rhs);

	eve::ast::type* type;

	if (base)
 	{
		type = base->type;

	} else if (id)
	{
		eve::ast::type_base* type_base = new eve::ast::type_base
		(
			id->location(),
			id->id->dup(),
			new std::vector<eve::ast::argument_type*>()
		);

		type_base->_parent = const_cast<eve::ast::argument_type*>(rhs);

		std::vector<eve::ast::node*> nodes = type_base->context().lookup(id->id);
		if (!nodes.empty())
			type_base->decl = nodes[0];

		type = type_base;
	} else
	{
		eve::driver d("");
		eve::target::cu cu;

		std::cerr << const_cast<eve::ast::argument_type*>(rhs)->translate(d, cu) << std::endl;
		assert(constant);
		return false;
	}

	return this->type->equals(type, out_bindings);
}

argument_type* argument_type_type::bind(std::map<std::string, eve::ast::argument_type*> bindings) const
{
	return new argument_type_type(this->location(), this->type->bind(bindings));
}

}
}

#endif