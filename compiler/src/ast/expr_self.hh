#ifndef EVE_AST_EXPR_SELF_IMPL
#define EVE_AST_EXPR_SELF_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_self::expr_self(const eve::location& location)
	: eve::ast::expr(location)
{
}

expr_self::~expr_self()
{
}

void expr_self::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string expr_self::translate(eve::driver& driver, eve::target::cu& cu)
{
	return this->wrap("(*this)", driver, cu);
}

}
}

#endif
