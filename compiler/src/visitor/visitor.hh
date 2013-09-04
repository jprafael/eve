#ifndef EVE_VISITOR_VISITOR_HH
#define EVE_VISITOR_VISITOR_HH

#include "../ast/ast.hh"

namespace eve
{
class driver;

namespace visitor
{

class visitor
{
	bool bottom_up;

public:
	visitor(bool bottom_up);
	virtual ~visitor();

	/*
		visit() calls handle() for the node and visit() for its children 
		handle() modifies/parses the node in question
	*/
#define BOTH(TYPE)						\
	virtual void handle(TYPE* node);	\
	virtual void visit(TYPE* node);

	BOTH(eve::ast::node)
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

#undef BOTH
};

/*
	visits imports to include new files (modules) to be be parsed and adopted by the program 
*/
class import_visitor : public visitor
{
	using visitor::handle;
	eve::driver& driver;

public:
	import_visitor(eve::driver& driver);
	virtual ~import_visitor();
	
	virtual void handle(eve::ast::import* node);
};

/*
	visits nodes that have a contexts to link them (bottom up)
*/
class context_visitor : public visitor
{
	using visitor::handle;
	eve::driver& driver;

public:
	context_visitor(eve::driver& driver);
	virtual ~context_visitor();

	virtual void handle(eve::ast::definition_function* node);
	virtual void handle(eve::ast::definition_type* node);
	virtual void handle(eve::ast::expr_lambda* node);
	virtual void handle(eve::ast::module* node);
	virtual void handle(eve::ast::stmt_finish* node);
	virtual void handle(eve::ast::stmt_for* node);
	virtual void handle(eve::ast::stmt_for_in* node);
	virtual void handle(eve::ast::stmt_if* node);
	virtual void handle(eve::ast::stmt_par* node);
	virtual void handle(eve::ast::stmt_while* node);
};

/*
	visits definitions to fill up context maps
*/
class definition_visitor : public visitor
{
	using visitor::handle;
	eve::driver& driver;

public:
	definition_visitor(eve::driver& driver);
	virtual ~definition_visitor();
	
	virtual void handle(eve::ast::definition_function_named* node);
	virtual void handle(eve::ast::definition_function_operator* node);
	virtual void handle(eve::ast::definition_type* node);
	virtual void handle(eve::ast::definition_variable* node);

	virtual void handle(eve::ast::parameter_function_named* node);
	virtual void handle(eve::ast::parameter_type_type* node);
	virtual void handle(eve::ast::parameter_type_constant* node);
};

/*
	visits imports again to add name aliases
*/
class import_resolve_visitor : public visitor
{
	using visitor::handle;
	eve::driver& driver;

public:
	import_resolve_visitor(eve::driver& driver);
	virtual ~import_resolve_visitor();
	
	virtual void handle(eve::ast::import_simple* node);
	virtual void handle(eve::ast::import_all* node);

	virtual void handle(eve::ast::module* node);
};


/*
	visits types to validate
		existing of the corresponding type declarations
		correct template instanciation
		type parameters resolvable
*/
class type_visitor : public visitor
{
	using visitor::handle;
	eve::driver& driver;

public:
	type_visitor(eve::driver& driver);
	virtual ~type_visitor();
	
	virtual void handle(eve::ast::type_base* node);
};

/*
	visits @ statements to assign a unique id
*/
class par_visitor : public visitor
{
	using visitor::handle;
	eve::driver& driver;

protected:
	int par_id;
	int finish_id;
	
public:
	par_visitor(eve::driver& driver);
	virtual ~par_visitor();
	
	virtual void handle(eve::ast::stmt_par* node);
	virtual void handle(eve::ast::stmt_finish* node);
};

class lambda_visitor : public visitor
{
	using visitor::handle;
	eve::driver& driver;

protected:
	int lambda_id;
	
public:
	lambda_visitor(eve::driver& driver);
	virtual ~lambda_visitor();
	
	virtual void handle(eve::ast::expr_lambda* node);
};

/* links expressions to the appropriate type */
class expr_visitor : public visitor
{
	using visitor::handle;
	eve::driver& driver;
	
public:
	expr_visitor(eve::driver& driver);
	virtual ~expr_visitor();
	
	/* calls the constructor */
	virtual void handle(eve::ast::definition_variable* node);

	/* fields */
	virtual void handle(eve::ast::expr_field* node);

	/* calls operators */
	virtual void handle(eve::ast::expr_prefix* node);
	virtual void handle(eve::ast::expr_incr* node);
	virtual void handle(eve::ast::expr_infix* node);
	virtual void handle(eve::ast::expr_assign* node);
	virtual void handle(eve::ast::expr_n_ary* node);

	virtual void handle(eve::ast::expr_id* node);
	virtual void handle(eve::ast::expr_constant* node);

	virtual void handle(eve::ast::expr_lambda* node);
	virtual void handle(eve::ast::stmt_on* node);

	virtual eve::ast::type* lookup(eve::ast::expr* base, eve::ast::id* id, bool allow_static = false);
};

class n_ary_visitor : public visitor
{
	using visitor::handle;
	eve::driver& driver;
	
public:
	n_ary_visitor(eve::driver& driver);
	virtual ~n_ary_visitor();	

	virtual void handle(eve::ast::expr_n_ary* node);
};

}
}

#endif