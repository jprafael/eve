#ifndef EVE_AST_STMT_IMPL
#define EVE_AST_STMT_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt::stmt(const eve::location& location)
	: eve::ast::node(location)
{
}

stmt::~stmt()
{
}

void stmt::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif
