#ifndef EVE_AST_STMT_DEFINITION_IMPL
#define EVE_AST_STMT_DEFINITION_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_definition::stmt_definition(const eve::location& location, eve::ast::definition* def)
	: eve::ast::stmt(location), def(def)
{
	this->adopt(def);
}

stmt_definition::~stmt_definition()
{
	delete this->def;
}

void stmt_definition::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

void stmt_definition::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	this->def->translate(driver, cu, scope);
}

}
}

#endif