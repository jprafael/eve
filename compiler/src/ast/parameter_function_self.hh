#ifndef EVE_AST_PARAMETER_FUNCTION_SELF_IMPL
#define EVE_AST_PARAMETER_FUNCTION_SELF_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

parameter_function_self::parameter_function_self(const eve::location& location)
	: eve::ast::parameter_function(location)
{
}

parameter_function_self::~parameter_function_self()
{
}

void parameter_function_self::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::ast::type* parameter_function_self::type() const
{
	std::vector<eve::ast::node*> matches = this->context().lookup("eve::ast::definition_type");
	assert(!matches.empty());

	return static_cast<eve::ast::definition_type*>(matches[0])->type();
}

}
}

#endif