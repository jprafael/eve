#ifndef EVE_N_ARY_VISITOR_IMPL
#define EVE_N_ARY_VISITOR_IMPL

#include "visitor.hh"
#include "../driver.hh"

namespace eve
{
namespace visitor
{

n_ary_visitor::n_ary_visitor(eve::driver& driver)
	: visitor(false), driver(driver)
{
}

n_ary_visitor::~n_ary_visitor()
{
}

void n_ary_visitor::handle(eve::ast::expr_n_ary* node)
{
	eve::ast::expr_id* id = dynamic_cast<eve::ast::expr_id*>(node->base);
	eve::ast::expr_field* field = dynamic_cast<eve::ast::expr_field*>(node->base);

	if (id)
	{
		node->id = id->id;
		node->base = NULL;

		for (size_t i = 0; i < node->_children.size(); i++)
			if (id == node->_children[i])
				node->_children.erase(node->_children.begin() + i);

		node->adopt(id->id);

		id->id = NULL;
		delete id;

	} else if (field)
	{
		node->id = field->id;
		node->base = field->expr;

		for (size_t i = 0; i < node->_children.size(); i++)
			if (field == node->_children[i])
				node->_children.erase(node->_children.begin() + i);

		node->adopt(node->id);
		node->adopt(node->base);

		field->id = NULL;
		field->expr = NULL;
		delete field;
	}
}

}
}

#endif
