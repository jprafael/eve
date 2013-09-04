#ifndef EVE_expr_visitor_IMPL
#define EVE_expr_visitor_IMPL

#include "visitor.hh"
#include "../driver.hh"

namespace eve
{
namespace visitor
{

expr_visitor::expr_visitor(eve::driver& driver)
	: visitor(true), driver(driver)
{
}

expr_visitor::~expr_visitor()
{
}

void expr_visitor::handle(eve::ast::definition_variable* node)
{
	/* member declarations do not matter (?) */
	if (dynamic_cast<eve::ast::definition_type*>(node->parent()))
		return;

	if (node->builtin)
		return;

	std::vector<const eve::ast::type*> arguments;
	std::vector<eve::ast::type*> out_arguments;

	arguments.push_back(node->type);

	for (size_t i = 0; i < node->arguments->size(); i++)
	{
		eve::ast::type* type = (*node->arguments)[i]->type();
		assert(type);

		arguments.push_back(type);
	}

	for (size_t i = 0; i < node->named_arguments->size(); i++)
	{
		eve::ast::type* type = (*node->named_arguments)[i]->expr->type();
		assert(type);

		arguments.push_back(type);
	}

	std::vector<eve::ast::type*> expected;

	if (!this->driver.checker().resolve(
			std::string("constructor"),
			node->type->base()->decl->context(),
			node->type->bindings(),
			arguments,
			expected,
			true))
	{
		if (!node->arguments->size() == 1 || node->type->requires_conversion((*node->arguments)[0]->type()))
			this->driver.error(node->location(), "variable definition does not match any constructor");
		
	} else
	{
		/* skip the "self" argument */
		size_t i = 1;
		
		for (size_t j = 0; j < node->arguments->size(); j++, i++)
			(*node->arguments)[j]->_expected = expected[i]; 

		for (size_t j = 0; j < node->named_arguments->size(); j++, i++ )
			(*node->named_arguments)[j]->expr->_expected = expected[i];
	}
}

void expr_visitor::handle(eve::ast::expr_field* node)
{
	node->_type = this->lookup(node->expr, node->id, true);
	assert(node->_type);
}

void expr_visitor::handle(eve::ast::expr_prefix* node)
{
	std::vector<const eve::ast::type*> arguments;
	std::vector<eve::ast::type*> expected;

	assert(node->expr->type());
	arguments.push_back(node->expr->type());

	node->_type = this->driver.checker().resolve
	(
		eve::ast::to_string(node->op),
		node->expr->type()->base()->decl->context(),
		node->expr->type()->bindings(),
		arguments,
		expected,
		true
	);

	assert(node->_type);
	node->expr->_expected = expected[0];
}

void expr_visitor::handle(eve::ast::expr_incr* node)
{
	std::vector<const eve::ast::type*> arguments;
	std::vector<eve::ast::type*> expected;

	arguments.push_back(node->expr->type());

	node->_type = this->driver.checker().resolve
	(
		eve::ast::to_string(node->op),
		node->expr->type()->base()->decl->context(),
		node->expr->type()->bindings(),
		arguments,
		expected,
		true
	);

	assert(node->_type);
	node->expr->_expected = expected[0];
}

void expr_visitor::handle(eve::ast::expr_infix* node)
{
	std::vector<const eve::ast::type*> arguments;
	std::vector<eve::ast::type*> expected;

	assert(node->left->type());
	assert(node->right->type());
	
	arguments.push_back(node->left->type());
	arguments.push_back(node->right->type());

	node->_type = this->driver.checker().resolve
	(
		eve::ast::to_string(node->op),
		node->left->type()->base()->decl->context(),
		node->left->type()->bindings(),
		arguments,
		expected,
		true
	);

	assert(node->_type);

	node->left->_expected = expected[0];
	node->right->_expected = expected[1];
}

void expr_visitor::handle(eve::ast::expr_assign* node)
{
	std::vector<const eve::ast::type*> arguments;
	std::vector<eve::ast::type*> expected;

	assert(node->left->type());
	assert(node->right->type());
	
	arguments.push_back(node->left->type());
	arguments.push_back(node->right->type());

	node->_type = this->driver.checker().resolve
	(
		eve::ast::to_string(node->op),
		node->left->type()->base()->decl->context(),
		node->left->type()->bindings(),
		arguments,
		expected,
		true
	);

	if (!node->_type)
	{
		/* default assign */
		node->_type = node->left->type();

	} else
	{
		node->left->_expected = expected[0];
		node->right->_expected = expected[1];
	}

	assert(node->_type);
}

void expr_visitor::handle(eve::ast::expr_n_ary* node)
{
	eve::ast::type* variable = NULL;
	std::vector<const eve::ast::type*> arguments;

	eve::symbol::context* context;
	std::map<std::string, eve::ast::argument_type*> bindings;
	std::string op;
	bool is_static;

	bool is_operator;

	if (node->id)
	{
		variable = this->lookup(node->base, node->id);

		if (variable)
			is_operator = true;

		else
			is_operator = false;

	} else
		is_operator = true;

	if (is_operator)
	{
		/* lookup operator () / [] */
		op = eve::ast::to_string(node->op);

		is_static = false;

		context = &variable->base()->decl->context();
		arguments.push_back(variable);
		bindings = variable->bindings();

		std::cerr << "OPERATOR" << std::endl;
				
	} else
	{
		/* lookup function */
		assert(node->op == eve::ast::op::n_ary_parens);
		assert(node->id);

		op = node->id->string();

		if (node->base)
		{
			eve::ast::type* type;

			/* inside type of node->base */
			if (node->base->_type->base()->is("static"))
			{
				/* X.f() */
				is_static = true;

				eve::ast::argument_type_type* arg = dynamic_cast<eve::ast::argument_type_type*>(node->base->_type->bindings()["T"]);
				assert(arg);
				type = arg->type;

			} else
			{
				/* x.f() */
				is_static = false;
				type = node->base->_type;

				arguments.push_back(type);
			}

			/* FIXME:
				analysis of templated functions should only occur when type parameters are known
				however we are currently only doing a bottom-up pass without binding information
			*/
			bindings = type->bindings();
			assert(dynamic_cast<eve::ast::definition_type*>(type->base()->decl));
			context = &type->base()->decl->context();

		} else
		{
			/* inside the current scope */
			is_static = true;
			context = &node->context();
		}
	}

	for (size_t i = 0; i < node->arguments->size(); i++)
	{
		eve::ast::type* type = (*node->arguments)[i]->type();
		assert(type);

		arguments.push_back(type);
	}

	for (size_t i = 0; i < node->named_arguments->size(); i++)
	{
		eve::ast::argument_function_named* named = (*node->named_arguments)[i]; 
		eve::ast::type* type = named->expr->type();
		assert(type);

		arguments.push_back(type);
	}

	std::vector<eve::ast::type*> expected;
	node->_type = this->driver.checker().resolve
	(
		op,
		*context,
		bindings,
		arguments,
		expected,
		true
	);

	std::cerr << op << std::endl;
	assert(node->_type);

	int i = 0;
	if (!is_static)
	{
		if (node->base)
			node->base->_expected = expected[0];

		/* TODO: expected to the field? */

		i++;
	}

	for (size_t j = 0; j < node->arguments->size(); j++, i++)
		(*node->arguments)[j]->_expected = expected[i];

	for (size_t j = 0; j < node->named_arguments->size(); j++, i++)
		(*node->named_arguments)[j]->expr->_expected = expected[i];
}

void expr_visitor::handle(eve::ast::expr_id* node)
{
	node->_type = this->lookup(NULL, node->id, true);
	assert(node->_type);
}

void expr_visitor::handle(eve::ast::expr_constant* node)
{
	node->_type = node->value->type(this->driver);
	assert(node->_type);
}

void expr_visitor::handle(eve::ast::expr_lambda* node)
{
	std::vector<eve::ast::type*> parameters;
	for (size_t i = 0; i < node->parameters->size(); i++)
		parameters.push_back((*node->parameters)[i]->type());

	node->_type = this->driver.checker().make_function(node->_ret, parameters);
}

void expr_visitor::handle(eve::ast::stmt_on* node)
{
	std::vector<eve::ast::type*> args;
	args.push_back(this->driver.checker().make_type(node->id));

	node->lambda->_expected = this->driver.checker().make_shared(
		this->driver.checker().make_function(
			this->driver.checker().make_bool(),
			args
		)
	);
}

eve::ast::type* expr_visitor::lookup(eve::ast::expr* base, eve::ast::id* id, bool allow_static)
{
	if (base)
	{
		/* field */
		std::map<std::string, eve::ast::argument_type*> bindings;
		std::vector<const eve::ast::type*> arguments;
		std::vector<eve::ast::type*> expected;
		eve::symbol::context* context;
		bool is_static;

		eve::ast::type* type;
		if (base->type()->base()->is("static"))
		{
			is_static = true;

			eve::ast::argument_type_type* arg = dynamic_cast<eve::ast::argument_type_type*>(base->type()->bindings()["T"]);
			assert(arg);
			type = arg->type;

		} else
		{
			is_static = false;

			type = base->type();
			arguments.push_back(type);
		}
		
		bindings = type->bindings();
		context = &type->base()->decl->context();

		eve::ast::type* ret = this->driver.checker().resolve
		(
			// is_static,
			id->string(),
			*context,
			bindings,
			arguments,
			expected,
			false
		);

		if (ret && !is_static && !expected.empty())
			base->_expected = expected[0];

		return ret;
	} else
	{
		eve::target::cu cu;

		/* simple variable */
		std::vector<eve::ast::node*> results = id->context().lookup(id);
		if (results.empty())
			return NULL;
		
		eve::ast::parameter_function_named* param = dynamic_cast<eve::ast::parameter_function_named*>(results[0]);
		eve::ast::definition_variable* var = dynamic_cast<eve::ast::definition_variable*>(results[0]);
		eve::ast::definition_type* type = dynamic_cast<eve::ast::definition_type*>(results[0]);

		if (var)
			return var->type;

		else if (param)
			return param->type();

		else if (allow_static)
		{	
			/* its something other than a variable (type, function, etc) */
			if (type)
				return this->driver.checker().make_static(type->type());
		}

		return NULL; 
	}
}

}
}

#endif
