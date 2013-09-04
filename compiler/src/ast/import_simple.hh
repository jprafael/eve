#ifndef EVE_AST_IMPORT_SIMPLE_IMPL
#define EVE_AST_IMPORT_SIMPLE_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

import_simple::import_simple(const eve::location& location, eve::ast::id* id)
	: eve::ast::import(location, id), alias( new eve::ast::simple_id(*(id->simple())) )
{
	this->adopt(alias);
}

import_simple::import_simple(const eve::location& location, eve::ast::id* id, eve::ast::simple_id* alias)
	: eve::ast::import(location, id), alias(alias)
{
	this->adopt(alias);
}

import_simple::~import_simple()
{
	delete this->alias;
}

void import_simple::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif
