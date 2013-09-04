#ifndef EVE_PAR_VISITOR_IMPL
#define EVE_PAR_VISITOR_IMPL

#include "visitor.hh"
#include "../driver.hh"

namespace eve
{
namespace visitor
{

par_visitor::par_visitor(eve::driver& driver)
	: visitor(true), driver(driver), par_id(0), finish_id(0)
{
}

par_visitor::~par_visitor()
{
}

void par_visitor::handle(eve::ast::stmt_par* node)
{
	node->id = this->par_id++;

	std::vector<eve::ast::node*> finishes = node->context().lookup(std::string("eve::ast::stmt_finish"));
	if (finishes.empty())
		return;

	assert(finishes.size() == 1);
	eve::ast::node *finish = finishes[0];

	/* make sure we are not inside 2+ levels deep tasks */
	std::vector<eve::ast::node*> pars = node->parent()->context().lookup("eve::ast::stmt_par");
	if (!pars.empty())
	{
		std::vector<eve::ast::node*> parent_finishes = pars[0]->context().lookup(std::string("eve::ast::stmt_finish"));		
		if (!parent_finishes.empty() && parent_finishes[0] == finishes[0])
			return;
	}

	eve::ast::stmt_finish* f = static_cast<eve::ast::stmt_finish*>(finish);
	node->set_finish(f);

	for (size_t i = 0; i < node->perms->size(); i++)
	{
		eve::ast::perm* perm = (*node->perms)[i];
		if (perm->shared)
			f->monitors.insert(perm->id->string());
	}
}

void par_visitor::handle(eve::ast::stmt_finish* node)
{
	node->id = this->finish_id++;
}

}
}

#endif
