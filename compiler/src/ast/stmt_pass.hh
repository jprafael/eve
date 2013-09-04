#ifndef EVE_AST_STMT_PASS_IMPL
#define EVE_AST_STMT_PASS_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_pass::stmt_pass(const eve::location& location)
	: eve::ast::stmt(location)
{
}

stmt_pass::~stmt_pass()
{
}

void stmt_pass::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}


void stmt_pass::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	/* nothing to do? */
}

}
}

#endif