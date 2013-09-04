#ifndef EVE_AST_AST_HH
#define EVE_AST_AST_HH

#include <vector>
#include <string>
#include <iostream>
#include <set>
#include <map>

#include "../location.hh"
#include "../symbol/symbol.hh"

namespace eve
{

/* forward declarations */
class driver;

namespace visitor
{
	class visitor;
	class import_visitor;
	class context_visitor;	
	class definition_visitor;
	class import_resolve_visitor;
	class type_visitor;
	class par_visitor;
	class lambda_visitor;
	class expr_visitor;
	class n_ary_visitor;
}

namespace checker
{
	class checker;
}

namespace target
{
	struct cu;
	struct task;
}

namespace ast
{
/* forward declarations */
class import;
class id;
class simple_id;
class qualified_id;
class definition;
class module;
class type;
class stmt;
class stmt_pass;
class expr;
class expr_lambda;
class expr_id;
class expr_n_ary;
class expr_constant;
class argument_type;
class argument_function;
class parameter_type;
class parameter_function;
class perm;
class stmt_finish;
class stmt_par;
class argument_function_named;
class argument_type_type;
class type_base;
class definition_variable;

/* enums */
enum op
{
	assign_eq,
	assign_shift_r,
	assign_shift_l,
	assign_add,
	assign_sub,
	assign_mul,
	assign_div,
	assign_mod,
	assign_and,
	assign_or,
	assign_bool_and,
	assign_bool_or,
	assign_xor,
	assign_power,

	incr_inc,
	incr_dec,

	prefix_plus,
	prefix_minus,
	prefix_not,
	prefix_bool_not,

	infix_add,
	infix_sub,
	infix_div,
	infix_xor,
	infix_and,
	infix_or,
	infix_mod,
	infix_shift_r,
	infix_shift_l,
	infix_bool_and,
	infix_bool_or,
	infix_power,
	infix_l,
	infix_g,
	infix_le,
	infix_ge,
	infix_ne,
	infix_ne3,
	infix_eq,
	infix_eq3,

	n_ary_parens,
	n_ary_bracks,

	asterisc // infix_mul and prefix_deref
};

enum constant_type
{
	type_bool,
	type_char,
	type_string,
	type_int,
	type_float
};

std::string to_string(const eve::ast::op& op);

enum perm_access
{
	access_none,
	access_copy,
	access_ref,
	access_const,
	access_full
};

/* declarations */
class node
{

public:
	eve::ast::node* _parent;
	std::vector<eve::ast::node*> _children;

	/* error reporting */
	eve::location _location;

	friend eve::visitor::context_visitor;
	friend eve::visitor::definition_visitor;
	friend eve::visitor::n_ary_visitor;

	node(const eve::location& location);
	virtual ~node();

public:
	void adopt(eve::ast::node* child);

	template <class T>
	void adopt_all(const std::vector<T*>& children);

	eve::ast::node*& parent();
	std::vector<eve::ast::node*>& children();

	virtual void accept(eve::visitor::visitor& visitor);
	virtual eve::symbol::context& context();
	virtual const eve::symbol::context& context() const;

	const eve::location& location() const;
};

class program : public node
{
	typedef std::map<std::string, eve::ast::module*> module_map_t;
	module_map_t modules;

protected:
	eve::symbol::context _context;

public:
	typedef eve::ast::node super;
	explicit program();
	virtual ~program();

	virtual void accept(eve::visitor::visitor& visitor);
	virtual eve::symbol::context& context();

	void translate(eve::driver& driver, eve::target::cu& cu);

	void add_module(const std::string& filename, eve::ast::module* module);
};

class constant : public node
{

protected:
	std::string* value;
	eve::ast::constant_type _type;

	friend eve::ast::expr_constant;

public:
	typedef eve::ast::node super;

	explicit constant(const eve::location& location, std::string* value, eve::ast::constant_type _type);
	virtual ~constant();

	virtual void accept(eve::visitor::visitor& visitor);

	std::string string() const;
	eve::ast::type* type(eve::driver& driver) const;

	eve::ast::constant* dup() const;
};

class id : public node
{

public:
	typedef eve::ast::node super;

	explicit id(const eve::location& location);
	virtual ~id();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual const eve::ast::simple_id* simple() const = 0;
	virtual const std::vector<eve::ast::simple_id*> ids() const = 0;

	virtual std::string string() const = 0;
	virtual eve::ast::id* dup() const = 0;
};

class simple_id : public id
{
protected:
	std::string* id;

public:
	typedef eve::ast::id super;

	explicit simple_id(const eve::location& location, std::string* id);
	explicit simple_id(std::string* id); /* constructor for builtins */
	virtual ~simple_id();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual const eve::ast::simple_id* simple() const;
	virtual const std::vector<eve::ast::simple_id*> ids() const;

	std::string string() const;

	virtual eve::ast::simple_id* dup() const;
};

class qualified_id : public id
{

protected:
	std::vector<eve::ast::simple_id*> _ids;

public:
	typedef eve::ast::id super;

	explicit qualified_id(const eve::location& location, eve::ast::simple_id* left, eve::ast::simple_id* right);
	explicit qualified_id(const eve::location& location, std::vector<eve::ast::simple_id*> ids);

	virtual ~qualified_id();

	virtual void accept(eve::visitor::visitor& visitor);

	void push(eve::ast::simple_id* id);

	virtual const eve::ast::simple_id* simple() const;
	virtual const std::vector<eve::ast::simple_id*> ids() const;
	std::string string() const;

	virtual eve::ast::qualified_id* dup() const;

};

class import : public node
{

protected:
	eve::ast::id* id;

	std::string filename;
	int offset;

	friend eve::visitor::import_visitor;
	friend eve::visitor::import_resolve_visitor;

public:
	typedef eve::ast::node super;

	explicit import(const eve::location& location, eve::ast::id* id);
	explicit import(const eve::location& location, eve::ast::id* id, eve::ast::simple_id* alias);

	virtual void accept(eve::visitor::visitor& visitor);

	virtual ~import();
};

class import_simple : public import
{

protected:
	eve::ast::simple_id* alias;

	friend eve::visitor::import_visitor;
	friend eve::visitor::import_resolve_visitor;

public:
	typedef eve::ast::import super;

	explicit import_simple(const eve::location& location, eve::ast::id* id);
	explicit import_simple(const eve::location& location, eve::ast::id* id, eve::ast::simple_id* alias);

	virtual void accept(eve::visitor::visitor& visitor);

	virtual ~import_simple();
};

class import_all : public import
{

protected:
	friend eve::visitor::import_visitor;
	friend eve::visitor::import_resolve_visitor;

public:
	typedef eve::ast::import super;

	explicit import_all(const eve::location& location, eve::ast::id* id);

	virtual void accept(eve::visitor::visitor& visitor);

	virtual ~import_all();
};

class definition : public node
{

protected:
	bool builtin;
	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::node super;

	explicit definition(const eve::location& location);
	virtual ~definition();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope) = 0;

	void set_builtin(bool value);

	/* returns the desired top level scope depending of definition type*/
	virtual std::vector<std::string>& top_scope(eve::target::cu& cu) const = 0;
};

class definition_function : public definition
{

protected:
	eve::ast::type* ret;
	std::vector<parameter_type*> *template_parameters;
	std::vector<parameter_function*>* function_parameters;
	std::vector<perm*>* perms;

	eve::symbol::context _context;

	size_t defaults;
	bool variadic;

	friend eve::ast::definition_variable;
	friend eve::checker::checker;

public:
	typedef eve::ast::node definition;

	explicit definition_function(const eve::location& location, eve::ast::type* rets, std::vector<parameter_type*>* template_parameters, std::vector<parameter_function*>* function_parameters, std::vector<perm*>* perms);
	virtual ~definition_function();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::symbol::context& context();

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope) = 0;

	virtual std::vector<std::string>& top_scope(eve::target::cu& cu) const;
};

class definition_function_named : public definition_function
{

protected:
	eve::ast::simple_id* id;

	friend eve::visitor::definition_visitor;

public:
	typedef eve::ast::definition_function super;

	explicit definition_function_named(const eve::location& location, eve::ast::type* type, std::vector<parameter_type*>* template_parameters, std::vector<parameter_function*>* parameters, std::vector<perm*>* perms, eve::ast::simple_id* id);
	virtual ~definition_function_named();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope) = 0;
};

class definition_function_named_explicit : public definition_function_named
{

protected:
	std::vector<stmt*>* stmts;

public:
	typedef eve::ast::definition_function_named super;
	explicit definition_function_named_explicit(const eve::location& location, eve::ast::type* type, std::vector<parameter_type*>* template_parameters, std::vector<parameter_function*>* function_parameters, std::vector<perm*>* perms, eve::ast::simple_id* id, std::vector<stmt*>* stmts);
	virtual ~definition_function_named_explicit();

	virtual void accept(eve::visitor::visitor& visitor);

	bool is_main();
	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope);
};

class definition_function_operator : public definition_function
{

protected:
	eve::ast::op op;

	friend eve::visitor::definition_visitor;

public:
	typedef eve::ast::definition_function super;

	explicit definition_function_operator(const eve::location& location, eve::ast::type* type, std::vector<parameter_type*>* template_parameters, std::vector<parameter_function*>* function_parameters, std::vector<perm*>* perms, eve::ast::op op);
	virtual ~definition_function_operator();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope) = 0;
};

class definition_function_operator_explicit : public definition_function_operator
{

protected:
	std::vector<stmt*>* stmts;

public:
	typedef eve::ast::definition_function_operator super;

	explicit definition_function_operator_explicit(const eve::location& location, eve::ast::type* type, std::vector<parameter_type*>* template_parameters, std::vector<parameter_function*>* function_parameters, std::vector<perm*>* perms, eve::ast::op op, std::vector<stmt*>* stmts);
	virtual ~definition_function_operator_explicit();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope);
};

class definition_variable : public definition
{

protected:
	eve::ast::type* type;
	eve::ast::simple_id* id;
	std::vector<eve::ast::expr*>* arguments;
	std::vector<eve::ast::argument_function_named*>* named_arguments;

	friend eve::visitor::definition_visitor;
	friend eve::visitor::expr_visitor;
	friend eve::checker::checker;
	friend eve::ast::perm;
	friend eve::ast::expr_id;

public:
	typedef eve::ast::definition super;

	explicit definition_variable(const eve::location& location, eve::ast::type* type, eve::ast::simple_id* id, eve::ast::expr* argument);
	explicit definition_variable(const eve::location& location, eve::ast::type* type, eve::ast::simple_id* id, std::vector<eve::ast::expr*>* arguments, std::vector<eve::ast::argument_function_named*>* named_arguments);
	virtual ~definition_variable();

	virtual void accept(eve::visitor::visitor& visitor);
	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope);

	virtual std::vector<std::string>& top_scope(eve::target::cu& cu) const;
};

class definition_type : public definition
{

protected:
	eve::ast::simple_id* id;
	std::vector<eve::ast::parameter_type*>* parameters;
	std::vector<eve::ast::type*>* parents;
	std::vector<eve::ast::definition*>* definitions;
	size_t defaults;
	
	eve::symbol::context _context;

	eve::ast::definition_type* generic;
	std::vector<eve::ast::definition_type*> specializations;

	friend eve::visitor::definition_visitor;
	friend eve::visitor::type_visitor;
	friend eve::ast::type_base;
	friend eve::checker::checker;
	
	/* constructor for builtins */
	explicit definition_type(eve::ast::simple_id*, std::vector<eve::ast::parameter_type*>* parameters);

public:	
	typedef eve::ast::definition super;

	explicit definition_type(const eve::location& location, eve::ast::simple_id* id, std::vector<eve::ast::parameter_type*>* parameters, std::vector<eve::ast::type*>* parents, std::vector<eve::ast::definition*>* definitions);
	virtual ~definition_type();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::symbol::context& context();

	/* true if all the parameters are free */
	bool is_generic() const;

	/* translate the type declaration and puts the necessary definitions inside the compilation unit*/
	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope);

	virtual std::vector<std::string>& top_scope(eve::target::cu& cu) const;
	virtual eve::ast::type* type() const;
};

class parameter : public node
{

public:
	typedef eve::ast::node super;

	explicit parameter(const eve::location& location);
	virtual ~parameter();

	virtual void accept(eve::visitor::visitor& visitor);
};

class parameter_function : public parameter
{

public:
	typedef eve::ast::parameter super;

	explicit parameter_function(const eve::location& location);
	virtual ~parameter_function();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::ast::type* type() const = 0;
};

class parameter_function_named : public parameter_function
{

protected:
	eve::ast::type* _type;
	eve::ast::simple_id* id;

	friend eve::ast::definition_function_named_explicit;
	friend eve::ast::definition_variable;
	friend eve::ast::perm;
	friend eve::ast::expr_id;
	friend eve::ast::expr_lambda;
	friend eve::visitor::definition_visitor;
public:
	typedef eve::ast::parameter_function super;

	explicit parameter_function_named(const eve::location& location, eve::ast::type* _type, eve::ast::simple_id* id);
	virtual ~parameter_function_named();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::ast::type* type() const;
};

class parameter_function_self : public parameter_function
{

public:
	typedef eve::ast::parameter_function super;

	explicit parameter_function_self(const eve::location& location);
	virtual ~parameter_function_self();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::ast::type* type() const;
};

class parameter_type : public parameter
{

protected:
	friend eve::visitor::type_visitor;

public:
	typedef eve::ast::parameter super;

	explicit parameter_type(const eve::location& location);
	virtual ~parameter_type();

	virtual void accept(eve::visitor::visitor& visitor);
};

class parameter_type_type : public parameter_type
{

protected:
	eve::ast::simple_id* id;
	eve::ast::type* type;
	bool elipsis;

	friend eve::visitor::definition_visitor;
	friend eve::visitor::type_visitor;
	friend eve::ast::definition_type;
	friend eve::ast::type_base;

public:
	typedef eve::ast::parameter_type super;

	explicit parameter_type_type(const eve::location& location, eve::ast::simple_id* id, eve::ast::type* type);
	explicit parameter_type_type(const eve::location& location, eve::ast::simple_id* id); /* elipsis constructor */

	virtual ~parameter_type_type();

	virtual void accept(eve::visitor::visitor& visitor);	
};

class parameter_type_constant : public parameter_type
{

protected:
	eve::ast::simple_id* id;
	eve::ast::constant* constant;
	bool elipsis;

	friend eve::visitor::definition_visitor;
	friend eve::visitor::type_visitor;
	friend eve::ast::definition_type;
	friend eve::ast::type_base;

public:
	typedef eve::ast::parameter_type super;

	explicit parameter_type_constant(const eve::location& location, eve::ast::simple_id* id, eve::ast::constant* constant);
	explicit parameter_type_constant(const eve::location& location, eve::ast::simple_id* id);
	virtual ~parameter_type_constant();

	virtual void accept(eve::visitor::visitor& visitor);	
};

/* class specialization by fixing a parameter to a special argument e.g: "class map<int>:" */
class parameter_type_specialization : public parameter_type
{

protected:
	eve::ast::argument_type* argument;

public:
	typedef eve::ast::parameter_type super;

	explicit parameter_type_specialization(const eve::location& location, eve::ast::argument_type* argument);
	virtual ~parameter_type_specialization();

	virtual void accept(eve::visitor::visitor& visitor);
};

class argument : public node
{

public:
	typedef eve::ast::node super;

	explicit argument(const eve::location& location);
	virtual ~argument();

	virtual void accept(eve::visitor::visitor& visitor);
};

class argument_type : public argument
{

public:
	typedef eve::ast::argument super;

	explicit argument_type(const eve::location& location);
	virtual ~argument_type();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual std::string translate(eve::driver& driver, eve::target::cu& cu) = 0;

	virtual argument_type* dup() const = 0;
	virtual bool equals(const eve::ast::argument_type* rhs, std::map<std::string, eve::ast::argument_type*>& bindings) const = 0;

	virtual argument_type* bind(std::map<std::string, eve::ast::argument_type*> bindings) const = 0;
};

class argument_type_constant : public argument_type
{

protected:
	eve::ast::constant* value;
	friend eve::visitor::type_visitor;
	friend eve::ast::type_base;
	friend eve::ast::constant;

public:
	typedef eve::ast::argument_type super;

	explicit argument_type_constant(const eve::location& location, eve::ast::constant* value);
	virtual ~argument_type_constant();

	virtual void accept(eve::visitor::visitor& visitor);

	std::string translate(eve::driver& driver, eve::target::cu& cu);

	virtual argument_type_constant* dup() const;
	virtual bool equals(const eve::ast::argument_type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const;

	virtual argument_type* bind(std::map<std::string, eve::ast::argument_type*> bindings) const;
};

class argument_type_id : public argument_type
{

protected:
	eve::ast::id* id;
	friend eve::ast::type_base;
	friend eve::ast::argument_type_type;
	friend eve::visitor::type_visitor;

public:
	typedef eve::ast::argument_type super;

	explicit argument_type_id(const eve::location& location, eve::ast::id* id);
	virtual ~argument_type_id();

	virtual void accept(eve::visitor::visitor& visitor);

	std::string translate(eve::driver& driver, eve::target::cu& cu);

	virtual argument_type_id* dup() const;
	virtual bool equals(const eve::ast::argument_type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const;
	virtual argument_type* bind(std::map<std::string, eve::ast::argument_type*> bindings) const;
};

class argument_type_type : public argument_type
{

protected:
	eve::ast::type* type;

	friend eve::ast::type_base;
	friend eve::ast::argument_type_id;
	friend eve::visitor::type_visitor;
	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::argument_type super;

	explicit argument_type_type(const eve::location& location, eve::ast::type* type);
	virtual ~argument_type_type();

	virtual void accept(eve::visitor::visitor& visitor);

	std::string translate(eve::driver& driver, eve::target::cu& cu);

	virtual argument_type_type* dup() const;
	virtual bool equals(const eve::ast::argument_type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const;
	virtual argument_type* bind(std::map<std::string, eve::ast::argument_type*> bindings) const;
};

class argument_function : public argument
{

public:
	typedef eve::ast::argument super;

	explicit argument_function(const eve::location& location);
	virtual ~argument_function();

	virtual void accept(eve::visitor::visitor& visitor);
};

class argument_function_expr : public argument_function
{

protected:
	eve::ast::expr* expr;

public:
	typedef eve::ast::argument_function super;

	explicit argument_function_expr(const eve::location& location, eve::ast::expr* expr);
	virtual ~argument_function_expr();

	virtual void accept(eve::visitor::visitor& visitor);
};

class argument_function_named : public argument_function
{

protected:
	eve::ast::simple_id* id;
	eve::ast::expr* expr;

	friend eve::ast::expr_n_ary;
	friend eve::ast::definition_variable;
	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::argument_function super;

	explicit argument_function_named(const eve::location& location, eve::ast::simple_id* id, eve::ast::expr* expr);
	virtual ~argument_function_named();

	virtual void accept(eve::visitor::visitor& visitor);
};

class type : public node
{

public:
	typedef eve::ast::node super;

	explicit type(const eve::location& location);
	virtual ~type();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual std::string translate(eve::driver& driver, eve::target::cu& cu) = 0;

	virtual eve::ast::type* dup() const = 0;
	virtual eve::ast::type_base* base() const = 0;

	virtual eve::ast::type* bind(std::map<std::string, eve::ast::argument_type*> bindings) const = 0;
	virtual std::map<std::string, eve::ast::argument_type*> bindings() const = 0;

	virtual bool is(const std::string& id) const = 0;

	virtual bool equals(const eve::ast::type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const = 0;
	virtual bool assignable(eve::driver& driver, const eve::ast::type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const = 0;
	virtual bool requires_conversion(const eve::ast::type* rhs) const = 0;
};

class type_base : public type
{

protected:
	eve::ast::id* id;
	std::vector<eve::ast::argument_type*>* arguments;

	/* this can be a definition_type or a type specialization */
	eve::ast::node* decl;

	friend eve::visitor::type_visitor;
	friend eve::visitor::expr_visitor;
	friend eve::ast::definition_type;
	friend eve::ast::argument_type_type;
	friend eve::checker::checker;

public:
	typedef eve::ast::type super;

	explicit type_base(const eve::location& location, eve::ast::id* id, std::vector<eve::ast::argument_type*>* arguments);
	virtual ~type_base();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);

	virtual eve::ast::type* dup() const;
	virtual eve::ast::type_base* base() const;

	virtual eve::ast::type* bind(std::map<std::string, eve::ast::argument_type*> bindings) const;
	virtual std::map<std::string, eve::ast::argument_type*> bindings() const;

	virtual bool is(const std::string& id) const;
	virtual bool equals(const eve::ast::type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const;
	virtual bool assignable(eve::driver& driver, const eve::ast::type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const;
	virtual bool requires_conversion(const eve::ast::type* rhs) const;
};

class type_elipsis : public type
{

protected:
	eve::ast::type* _base;

	friend definition_variable;
	
public:
	typedef eve::ast::type super;

	explicit type_elipsis(const eve::location& location, eve::ast::type* base);
	virtual ~type_elipsis();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);

	virtual eve::ast::type* dup() const;
	virtual eve::ast::type_base* base() const;

	virtual eve::ast::type* bind(std::map<std::string, eve::ast::argument_type*> bindings) const;
	virtual std::map<std::string, eve::ast::argument_type*> bindings() const;

	virtual bool is(const std::string& id) const;

	virtual bool equals(const eve::ast::type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const;
	virtual bool assignable(eve::driver& driver, const eve::ast::type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const;
	virtual bool requires_conversion(const eve::ast::type* rhs) const;
};

class stmt : public node
{

public:
	typedef eve::ast::node super;

	explicit stmt(const eve::location& location);
	virtual ~stmt();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel) = 0;
};

class stmt_pass : public stmt
{

public:
	typedef eve::ast::stmt super;

	explicit stmt_pass(const eve::location& location);
	virtual ~stmt_pass();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);
};

class stmt_definition : public stmt
{

protected:
	eve::ast::definition* def;
	
public:
	typedef eve::ast::stmt super;

	explicit stmt_definition(const eve::location& location, eve::ast::definition* def);
	virtual ~stmt_definition();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);
};

class stmt_expr : public stmt
{

protected:
	eve::ast::expr* expr;

public:
	typedef eve::ast::stmt super;

	explicit stmt_expr(const eve::location& location, eve::ast::expr* expr);
	virtual ~stmt_expr();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);
};

class stmt_if : public stmt
{

protected:
	eve::ast::expr* expr;
	std::vector<eve::ast::stmt*>* then_block;
	std::vector<eve::ast::stmt*>* else_block;

	eve::symbol::context _context;

public:
	typedef eve::ast::stmt super;

	explicit stmt_if(const eve::location& location, eve::ast::expr* expr, std::vector<eve::ast::stmt*>* then_block, std::vector<eve::ast::stmt*>* else_block);
	virtual ~stmt_if();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::symbol::context& context();

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);
};

class stmt_while : public stmt
{

protected:
	eve::ast::expr* expr;
	std::vector<eve::ast::stmt*>* block;

	eve::symbol::context _context;

public:
	typedef eve::ast::stmt super;

	explicit stmt_while(const eve::location& location, eve::ast::expr* expr, std::vector<eve::ast::stmt*>* block);
	virtual ~stmt_while();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::symbol::context& context();

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);
};

class stmt_for : public stmt
{

protected:
	bool parallel;

	eve::ast::expr* init;
	eve::ast::expr* cond;
	eve::ast::expr* incr;

	std::vector<eve::ast::stmt*>* block;
	std::vector<eve::ast::perm*>* perms;

	eve::symbol::context _context;

public:
	typedef eve::ast::stmt super;

	explicit stmt_for(const eve::location& location, eve::ast::expr* init, eve::ast::expr* cond, eve::ast::expr* incr, std::vector<eve::ast::stmt*>* block);
	explicit stmt_for(const eve::location& location, eve::ast::expr* init, eve::ast::expr* cond, eve::ast::expr* incr, std::vector<eve::ast::stmt*>* block, std::vector<eve::ast::perm*>* perms);
	virtual ~stmt_for();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::symbol::context& context();

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);
};

class stmt_for_in : public stmt
{

protected:
	bool parallel;

	eve::ast::simple_id* id;
	eve::ast::expr* expr;

	std::vector<eve::ast::stmt*>* block;
	std::vector<eve::ast::perm*>* perms;

	eve::symbol::context _context;

public:
	typedef eve::ast::stmt super;

	explicit stmt_for_in(const eve::location& location, eve::ast::simple_id* id, eve::ast::expr* expr, std::vector<eve::ast::stmt*>* block);
	explicit stmt_for_in(const eve::location& location, eve::ast::simple_id* id, eve::ast::expr* expr, std::vector<eve::ast::stmt*>* block, std::vector<eve::ast::perm*>* perms);
	virtual ~stmt_for_in();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::symbol::context& context();

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);
};

class stmt_return : public stmt
{

protected:
	eve::ast::expr* expr;

public:
	typedef eve::ast::stmt super;

	explicit stmt_return(const eve::location& location, eve::ast::expr* expr);
	virtual ~stmt_return();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);
};

class stmt_par : public stmt
{

protected:
	std::vector<eve::ast::perm*>* perms;
	std::vector<eve::ast::stmt*>* stmts;

	eve::symbol::context _context;
	eve::ast::stmt_finish* finish;
	int id;

	friend eve::visitor::par_visitor;
	friend eve::ast::stmt_finish;

public:
	typedef eve::ast::stmt super;

	explicit stmt_par(const eve::location& location, std::vector<eve::ast::perm*>* perms, std::vector<eve::ast::stmt*>* stmts);
	virtual ~stmt_par();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::symbol::context& context();

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);

	void set_finish(eve::ast::stmt_finish* finish);
};

class stmt_on : public stmt
{

protected:
	eve::ast::expr* expr;
	eve::ast::simple_id* id;
	eve::ast::expr* lambda;

	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::stmt super;

	explicit stmt_on(const eve::location& location, eve::ast::expr* expr, eve::ast::simple_id* id, eve::ast::expr* lambda);
	virtual ~stmt_on();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);
};

class stmt_finish : public stmt
{

protected:
	std::vector<eve::ast::stmt*>* stmts;

	eve::symbol::context _context;
	int id;
	std::set<std::string> monitors;

	std::vector<eve::ast::stmt_par*> par_stmts;

	friend eve::visitor::par_visitor;
	friend eve::ast::stmt_par;
	friend eve::ast::perm;
	
public:
	typedef eve::ast::stmt super;

	explicit stmt_finish(const eve::location& location, std::vector<eve::ast::stmt*>* stmts);
	virtual ~stmt_finish();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::symbol::context& context();

	virtual void translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel);

	bool single_task();
};

class module : public node
{

protected:
	std::vector<import*>* import_list;
	std::vector<definition*>* decl_list;

	eve::symbol::context _context;

public:
	typedef eve::ast::node super;

	explicit module(const eve::location& location, std::vector<import*>* import_list, std::vector<definition*>* decl_list);
	virtual ~module();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::symbol::context& context();

	void translate(eve::driver& driver, eve::target::cu& cu);
};

class expr : public node
{

protected:
	/* filled in by expr_visitor */
	eve::ast::type* _type;
	eve::ast::type* _expected;

	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::node super;

	explicit expr(const eve::location& location);
	virtual ~expr();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual std::string translate(eve::driver& driver, eve::target::cu& cu) = 0;

	virtual eve::ast::type* type();

	virtual std::string wrap(std::string value, eve::driver& driver, eve::target::cu& cu);
};

class expr_constant : public expr
{

protected:
	eve::ast::constant* value;

	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::expr super;

	explicit expr_constant(const eve::location& location, eve::ast::constant* value);
	virtual ~expr_constant();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);
};

class expr_self : public expr
{

public:
	typedef eve::ast::expr super;

	explicit expr_self(const eve::location& location);
	virtual ~expr_self();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);
};

class expr_field: public expr
{

protected:
	eve::ast::expr* expr;
	eve::ast::simple_id* id;

	friend eve::visitor::expr_visitor;
	friend eve::visitor::n_ary_visitor;

public:
	typedef eve::ast::expr super;

	explicit expr_field(const eve::location& location, eve::ast::expr* expr, eve::ast::simple_id* id);
	virtual ~expr_field();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);
};

class expr_unary : public expr
{

protected:
	eve::ast::expr* expr;

	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::expr super;

	explicit expr_unary(const eve::location& location, eve::ast::expr* expr);
	virtual ~expr_unary();

	virtual void accept(eve::visitor::visitor& visitor);
};

class expr_prefix : public expr_unary
{

protected:
	eve::ast::op op;

	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::expr_unary super;

	explicit expr_prefix(const eve::location& location, eve::ast::expr* expr, eve::ast::op op);
	virtual ~expr_prefix();

	virtual void accept(eve::visitor::visitor& visitor);
	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);
};

class expr_incr : public expr_unary
{

protected:
	eve::ast::op op;
	bool prefix;

	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::expr_unary super;

	explicit expr_incr(const eve::location& location, eve::ast::expr* expr, eve::ast::op op, bool prefix);
	virtual ~expr_incr();

	virtual void accept(eve::visitor::visitor& visitor);
	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);
};

class expr_binary : public expr
{

protected:
	eve::ast::expr* left;
	eve::ast::expr* right;

	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::expr super;

	explicit expr_binary(const eve::location& location, eve::ast::expr* left, eve::ast::expr* right);
	virtual ~expr_binary();

	virtual void accept(eve::visitor::visitor& visitor);
};

class expr_infix : public expr_binary
{

protected:
	eve::ast::op op;

	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::expr_binary super;

	explicit expr_infix(const eve::location& location, eve::ast::expr* left, eve::ast::expr* right, eve::ast::op op);
	virtual ~expr_infix();

	virtual void accept(eve::visitor::visitor& visitor);
	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);
};

class expr_assign : public expr_binary
{

protected:
	eve::ast::op op;

	friend eve::visitor::expr_visitor;

public:
	typedef eve::ast::expr_binary super;

	explicit expr_assign(const eve::location& location, eve::ast::expr* left, eve::ast::expr* right, eve::ast::op);
	virtual ~expr_assign();

	virtual void accept(eve::visitor::visitor& visitor);
	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);
};

class expr_id : public expr
{

protected:
	eve::ast::id* id;

	friend eve::visitor::expr_visitor;
	friend eve::visitor::n_ary_visitor;

public:
	typedef eve::ast::expr super;

	explicit expr_id(const eve::location& location, eve::ast::id* id);
	virtual ~expr_id();

	virtual void accept(eve::visitor::visitor& visitor);
	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);
};

class expr_n_ary : public expr
{

protected:
	eve::ast::expr* base;
	eve::ast::op op;
	std::vector<eve::ast::expr*>* arguments;
	std::vector<eve::ast::argument_function_named*>* named_arguments;

	eve::ast::id* id;

	friend eve::visitor::expr_visitor;
	friend eve::visitor::n_ary_visitor;
	
public:
	typedef eve::ast::expr super;

	explicit expr_n_ary(const eve::location& location, eve::ast::expr* base, eve::ast::op op, std::vector<eve::ast::expr*>* arguments, std::vector<eve::ast::argument_function_named*>* named_arguments);
	virtual ~expr_n_ary();

	virtual void accept(eve::visitor::visitor& visitor);
	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);
};

class expr_lambda : public expr
{

protected:
	eve::ast::type* _ret;
	std::vector<eve::ast::parameter_function*>* parameters;
	std::vector<eve::ast::perm*>* perms;
	std::vector<eve::ast::stmt*>* stmts;

	eve::symbol::context _context;
	int id;

	friend eve::visitor::lambda_visitor;
	friend eve::visitor::expr_visitor;
	
public:
	typedef eve::ast::expr super;

	explicit expr_lambda(const eve::location& location, eve::ast::type* type, std::vector<eve::ast::parameter_function*>* parameters, std::vector<eve::ast::perm*>* perms, eve::ast::expr* expr);
	explicit expr_lambda(const eve::location& location, eve::ast::type* type, std::vector<eve::ast::parameter_function*>* parameters, std::vector<eve::ast::perm*>* perms, std::vector<eve::ast::stmt*>* stmts);
	virtual ~expr_lambda();

	virtual void accept(eve::visitor::visitor& visitor);

	virtual eve::symbol::context& context();
	virtual std::string translate(eve::driver& driver, eve::target::cu& cu);
};

class perm : public node
{

protected:
	eve::ast::id* id;
	eve::ast::perm_access access;
	bool shared;

	friend eve::ast::stmt_par;
	friend eve::ast::expr_id;
	friend eve::visitor::par_visitor;

public:
	typedef eve::ast::node super;

	explicit perm(const eve::location& location, eve::ast::id* id, eve::ast::perm_access access);
	virtual ~perm();

	virtual void accept(eve::visitor::visitor& visitor);
	virtual void translate(eve::driver& driver, eve::target::cu& cu, eve::target::task& task, std::vector<std::string>& arguments, eve::ast::stmt_finish* finish);
};

}
}

#endif
