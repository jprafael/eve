#ifndef EVE_AST_EXPR_CONSTANT_IMPL
#define EVE_AST_EXPR_CONSTANT_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_constant::expr_constant(const eve::location& location, eve::ast::constant* value)
	: eve::ast::expr(location), value(value)
{
	this->adopt(this->value);
}

expr_constant::~expr_constant()
{
	delete this->value;
}

void expr_constant::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string expr_constant::translate(eve::driver& driver, eve::target::cu& cu)
{
	std::string ret;

	bool shared = this->_expected && this->_expected->is("shared");

	if (!shared)
	{
		if (this->_expected)
			ret += this->_expected->translate(driver, cu) + "(";

		else
			ret += this->_type->translate(driver, cu) + "(";
	}

	ret += this->value->string();

	if (!shared)
		ret += ")";

	return this->wrap(ret, driver, cu);
}

}
}

#endif
