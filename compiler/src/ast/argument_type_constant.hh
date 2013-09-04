#ifndef EVE_AST_ARGUMENT_TYPE_CONSTANT_IMPL
#define EVE_AST_ARGUMENT_TYPE_CONSTANT_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

argument_type_constant::argument_type_constant(const eve::location& location, eve::ast::constant* value)
	: eve::ast::argument_type(location), value(value)
{
	this->adopt(this->value);
}

argument_type_constant::~argument_type_constant()
{
	delete this->value;
}

void argument_type_constant::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string argument_type_constant::translate(eve::driver& driver, eve::target::cu& cu)
{
	return this->value->string();
}

argument_type_constant* argument_type_constant::dup() const
{
	argument_type_constant* constant = new argument_type_constant(this->location(), this->value->dup());
	constant->_parent = this->_parent;
	return constant;
}

bool argument_type_constant::equals(const eve::ast::argument_type* rhs, std::map<std::string, eve::ast::argument_type*>& bindings) const
{
	if (this == rhs)
		return true;

	const eve::ast::argument_type_constant* base = dynamic_cast<const eve::ast::argument_type_constant*>(rhs);
	if (!base)
		return false;

	return this->value->string() == base->value->string();
}

argument_type* argument_type_constant::bind(std::map<std::string, eve::ast::argument_type*> out_bindings) const
{
	return this->dup();
}

}
}

#endif