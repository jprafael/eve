#ifndef EVE_AST_IMPORT_ALL_IMPL
#define EVE_AST_IMPORT_ALL_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

import_all::import_all(const eve::location& location, eve::ast::id* id)
	: eve::ast::import(location, id)
{
}

import_all::~import_all()
{
}

void import_all::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif
