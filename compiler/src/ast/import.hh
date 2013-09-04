#ifndef EVE_AST_IMPORT_IMPL
#define EVE_AST_IMPORT_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

import::import(const eve::location& location, eve::ast::id* id)
	: eve::ast::node(location), id(id), offset(-1)
{
	this->adopt(id);
}

import::import(const eve::location& location, eve::ast::id* id, eve::ast::simple_id* alias)
	: eve::ast::node(location), id(id), offset(-1)
{
	this->adopt(id);
}

import::~import()
{
	delete this->id;
}

void import::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif
