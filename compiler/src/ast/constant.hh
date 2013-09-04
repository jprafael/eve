#ifndef EVE_AST_CONSTANT_IMPL
#define EVE_AST_CONSTANT_IMPL

namespace eve
{
namespace ast
{

constant::constant(const eve::location& location, std::string* value, eve::ast::constant_type _type)
	: eve::ast::node(location), value(value), _type(_type)
{
}

constant::~constant()
{
}

void constant::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string constant::string() const
{
	return *this->value;
}

eve::ast::type* constant::type(eve::driver& driver) const
{
	std::vector<eve::ast::node*> results;

	switch (this->_type)
	{
		case type_bool:
			results = this->context().lookup("bool");
		break;

		case type_char:
			results = this->context().lookup("char");
		break;

		case type_string:
			results = this->context().lookup("string");
		break;

		case type_int:
			results = this->context().lookup("int");
		break;

		case type_float:
			results = this->context().lookup("float");
		break;
	}

	assert(results.size() == 1);

	eve::ast::definition_type* decl = dynamic_cast<eve::ast::definition_type*>(results[0]);
	assert(decl);

	eve::ast::type* type = decl->type();

	if (this->_type == type_int || this->_type == type_float)
	{
		driver.debug("TODO: verify int/float size");

		std::map<std::string, eve::ast::argument_type*> bindings;

		eve::ast::constant* c = new eve::ast::constant(
			this->location(),
			new std::string("32"),
			eve::ast::constant_type::type_int
		);
		c->_parent = this->_parent;

		eve::ast::argument_type_constant* arg = new eve::ast::argument_type_constant(this->location(), c);
		arg->_parent = this->_parent;

		bindings["B"] = arg;
		type = type->bind(bindings);
	}

	return type;
}

eve::ast::constant* constant::dup() const
{
	eve::ast::constant* node = new eve::ast::constant(this->location(), new std::string(*this->value), this->_type);
	node->_parent = this->_parent;
	return node;
}

}
}

#endif