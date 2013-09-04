#ifndef EVE_AST_DEFINITION_IMPL
#define EVE_AST_DEFINITION_IMPL

namespace eve
{
namespace ast
{

definition::definition(const eve::location& location)
	: eve::ast::node(location), builtin(false)
{
}

definition::~definition()
{
}

void definition::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

void definition::set_builtin(bool value)
{
	this->builtin = value;
}

}
}

#endif