#ifndef EVE_VISITOR_CONTEXT_VISITOR_IMPL
#define EVE_VISITOR_CONTEXT_VISITOR_IMPL

#include "visitor.hh"
#include "../driver.hh"

namespace eve
{
namespace visitor
{

context_visitor::context_visitor(eve::driver& driver)
	: visitor(false), driver(driver)
{
}

context_visitor::~context_visitor()
{
}

#define HANDLE(TYPE) 											\
	void context_visitor::handle(TYPE* node)					\
	{															\
		node->context().parent = &node->_parent->context();		\
		node->context().insert(std::string(#TYPE), node);		\
	}

HANDLE(eve::ast::definition_function);
HANDLE(eve::ast::definition_type);
HANDLE(eve::ast::expr_lambda);
HANDLE(eve::ast::module);
HANDLE(eve::ast::stmt_finish);
HANDLE(eve::ast::stmt_for);
HANDLE(eve::ast::stmt_for_in);
HANDLE(eve::ast::stmt_if);
HANDLE(eve::ast::stmt_par);
HANDLE(eve::ast::stmt_while);

}
}

#endif
