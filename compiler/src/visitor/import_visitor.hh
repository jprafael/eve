#ifndef EVE_VISITOR_IMPORT_VISITOR_IMPL
#define EVE_VISITOR_IMPORT_VISITOR_IMPL

#include "visitor.hh"
#include "../driver.hh"

namespace eve
{
namespace visitor
{

import_visitor::import_visitor(eve::driver& driver)
	: visitor(true), driver(driver)
{
}

import_visitor::~import_visitor()
{
}

void import_visitor::handle(eve::ast::import* node)
{
	node->offset = this->driver.import(node->id, node->filename);

	if (node->offset < 0)
		this->driver.error(node->location(), "failed to resolve import");
}

}
}

#endif
