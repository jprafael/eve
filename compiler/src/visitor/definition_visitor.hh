#ifndef EVE_VISITOR_DEFINITION_VISITOR_IMPL
#define EVE_VISITOR_DEFINITION_VISITOR_IMPL

#include "visitor.hh"
#include "../driver.hh"

namespace eve
{
namespace visitor
{

definition_visitor::definition_visitor(eve::driver& driver)
	: visitor(false), driver(driver)
{
}

definition_visitor::~definition_visitor()
{
}

void definition_visitor::handle(eve::ast::definition_function_named* node)
{
	/* normal function */
	if (node->id)
	{
		node->_parent->context().insert(node->id, node);
		return;
	}
	
	node->_parent->context().insert("constructor", node);
	std::vector<eve::ast::node*> types = node->context().lookup("eve::ast::definition_type");
	assert(!types.empty());

	eve::ast::definition_type* def = dynamic_cast<eve::ast::definition_type*>(types[0]);
	assert(def);

	node->ret = def->type();
}

void definition_visitor::handle(eve::ast::definition_function_operator* node)
{
	node->_parent->context().insert(eve::ast::to_string(node->op), node);
}

void definition_visitor::handle(eve::ast::definition_type* node)
{
	if (node->is_generic())
	{
		node->_parent->context().insert(node->id, node);

		/* count the number of arguments with default values */
		bool end = false;

		for (size_t i = 0; i < node->parameters->size(); i++)
		{
			eve::ast::parameter_type* param = (*node->parameters)[i];
			eve::ast::parameter_type_type* param_type = dynamic_cast<eve::ast::parameter_type_type*>(param);
			eve::ast::parameter_type_constant* param_constant = dynamic_cast<eve::ast::parameter_type_constant*>(param);

			if (param_type && param_type->type)
			{
				node->defaults++;
				if (end)
					this->driver.error(node->location(), "default type after end of parameters with default values");

			} else if (param_constant && param_constant->constant)
			{
				node->defaults++;
				if (end)
					this->driver.error(node->location(), "default constant after end of parameters with default values");

			} else 
				end = true;
		}
	}
}

void definition_visitor::handle(eve::ast::definition_variable* node)
{
	node->_parent->context().insert(node->id, node);
}

void definition_visitor::handle(eve::ast::parameter_function_named* node)
{
	node->_parent->context().insert(node->id, node);
}

void definition_visitor::handle(eve::ast::parameter_type_type* node)
{
	node->_parent->context().insert(node->id, node);
}

void definition_visitor::handle(eve::ast::parameter_type_constant* node)
{
	node->_parent->context().insert(node->id, node);
}

}
}

#endif
