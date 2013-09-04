#ifndef EVE_VISITOR_VISITOR_IMPL
#define EVE_VISITOR_VISITOR_IMPL

#include "visitor.hh"

namespace eve
{
namespace visitor
{

visitor::visitor(bool bottom_up)
	: bottom_up(bottom_up)
{
}

visitor::~visitor()
{
}

#define VISIT(TYPE) 											\
	void visitor::visit(TYPE* node) 							\
	{															\
		if (!this->bottom_up)									\
			this->handle(node);									\
																\
		for (size_t i = 0; i < node->children().size(); i++)	\
			node->children()[i]->accept(*this);					\
																\
		if (this->bottom_up)									\
			this->handle(node);									\
	}															\

#define HANDLE(TYPE)											\
	void visitor::handle(TYPE* node)							\
	{															\
		this->handle((TYPE::super*) node);						\
	}

#define BOTH(TYPE)												\
	VISIT(TYPE)													\
	HANDLE(TYPE)

/* node is the base type and is handle specially */
VISIT(eve::ast::node)

void visitor::handle(eve::ast::node* node)
{
}

/* all other nodes*/
BOTH(eve::ast::program)

BOTH(eve::ast::id)
BOTH(eve::ast::simple_id)
BOTH(eve::ast::qualified_id)

BOTH(eve::ast::import)
BOTH(eve::ast::import_simple)
BOTH(eve::ast::import_all)

BOTH(eve::ast::definition)
BOTH(eve::ast::definition_function)
BOTH(eve::ast::definition_function_named)
BOTH(eve::ast::definition_function_named_explicit)
BOTH(eve::ast::definition_function_operator)
BOTH(eve::ast::definition_function_operator_explicit)
BOTH(eve::ast::definition_variable)
BOTH(eve::ast::definition_type)

BOTH(eve::ast::parameter)
BOTH(eve::ast::parameter_function)
BOTH(eve::ast::parameter_function_named)
BOTH(eve::ast::parameter_function_self)
BOTH(eve::ast::parameter_type)
BOTH(eve::ast::parameter_type_type)
BOTH(eve::ast::parameter_type_constant)

BOTH(eve::ast::argument)
BOTH(eve::ast::argument_type)
BOTH(eve::ast::argument_type_constant)
BOTH(eve::ast::argument_type_id)
BOTH(eve::ast::argument_type_type)
BOTH(eve::ast::argument_function)
BOTH(eve::ast::argument_function_expr)
BOTH(eve::ast::argument_function_named)

BOTH(eve::ast::type)
BOTH(eve::ast::type_base)
BOTH(eve::ast::type_elipsis)

BOTH(eve::ast::stmt)
BOTH(eve::ast::stmt_pass)
BOTH(eve::ast::stmt_definition)
BOTH(eve::ast::stmt_expr)
BOTH(eve::ast::stmt_if)
BOTH(eve::ast::stmt_while)
BOTH(eve::ast::stmt_for)
BOTH(eve::ast::stmt_for_in)
BOTH(eve::ast::stmt_return)
BOTH(eve::ast::stmt_par)
BOTH(eve::ast::stmt_on)
BOTH(eve::ast::stmt_finish)

BOTH(eve::ast::module)

BOTH(eve::ast::expr)
BOTH(eve::ast::expr_constant)
BOTH(eve::ast::expr_self)
BOTH(eve::ast::expr_field)
BOTH(eve::ast::expr_unary)
BOTH(eve::ast::expr_prefix)
BOTH(eve::ast::expr_incr)
BOTH(eve::ast::expr_binary)
BOTH(eve::ast::expr_infix)
BOTH(eve::ast::expr_assign)
BOTH(eve::ast::expr_id)
BOTH(eve::ast::expr_n_ary)
BOTH(eve::ast::expr_lambda)

BOTH(eve::ast::perm)

}
}

#undef VISIT
#undef HANDLE
#undef BOTH


#endif
