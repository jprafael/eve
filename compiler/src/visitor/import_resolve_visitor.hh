#ifndef EVE_VISITOR_IMPORT_RESOLVE_VISITOR_IMPL
#define EVE_VISITOR_IMPORT_RESOLVE_VISITOR_IMPL

#include <cassert>

#include "visitor.hh"
#include "../driver.hh"

namespace eve
{
namespace visitor
{

import_resolve_visitor::import_resolve_visitor(eve::driver& driver)
	: visitor(true), driver(driver)
{
}

import_resolve_visitor::~import_resolve_visitor()
{
}

void import_resolve_visitor::handle(eve::ast::import_simple* node)
{
	std::vector<eve::ast::simple_id*> ids = node->id->ids();
	std::vector<eve::ast::node*> targets = node->context().lookup(node->filename);
	assert(targets.size() == 1);

	eve::ast::node* target = targets[0];

	for (unsigned int i = node->offset + 1; i < ids.size(); i++)
	{
		targets = target->context().lookup(ids[i]);

		if (targets.empty())
		{
			this->driver.error(node->location(), "unable to find symbol \"" + ids[i]->string() + "\" in " + node->filename);
			return;
		}

		assert(targets.size() == 1);
		target = targets[0];
	}

	node->context().insert(node->alias, target);
}

void import_resolve_visitor::handle(eve::ast::import_all* node)
{
	std::vector<eve::ast::simple_id*> ids = node->id->ids();
	std::vector<eve::ast::node*> targets = node->context().lookup(node->filename);

	assert(targets.size() == 1);
	eve::ast::node* target = targets[0];

	for (unsigned int i = node->offset + 1; i < ids.size(); i++)
	{
		targets = target->context().lookup(ids[i]);

		if (targets.empty())
		{
			this->driver.error(node->location(), "unable to find symbol \"" + ids[i]->string() + "\" in " + node->filename);
			return;
		}

		assert(targets.size() == 1);
		target = targets[0];
	}

	eve::symbol::context::symbol_table& symbols = target->context().symbols; 
	for (eve::symbol::context::symbol_table::iterator i = symbols.begin(); i != symbols.end(); ++i)
		node->context().insert(i->first, i->second);
}

void import_resolve_visitor::handle(eve::ast::module* node)
{
	/* TODO: add builtins to all modules (including other builtin modules, but excluding themselves) */
	typedef std::map<std::string, eve::ast::module*> builtin_map;
	std::map<std::string, eve::ast::node*> builtins;

	for (builtin_map::iterator i = this->driver.builtins.begin(); i != this->driver.builtins.end(); ++i)
	{
		eve::ast::module* module = i->second;
		eve::symbol::context::symbol_table& symbols = module->context().symbols; 

		for (eve::symbol::context::symbol_table::iterator j = symbols.begin(); j != symbols.end(); ++j)
			builtins[j->first] = j->second;
	}

	for (auto it = builtins.begin(); it != builtins.end(); ++it)
	{
		if (node->context().lookup(it->first).empty())
			node->context().insert(it->first, it->second);
	}
}

}
}

#endif
