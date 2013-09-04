#ifndef EVE_CHECKER_IMPL
#define EVE_CHECKER_IMPL

#include "checker.hh"

namespace eve
{
namespace checker
{

checker::checker(eve::driver& driver)
	: driver(driver)
{
}

checker::~checker()
{
}

eve::ast::type* checker::resolve
(
	std::string name,
	const eve::symbol::context& context,
	std::map<std::string, eve::ast::argument_type*> bindings,
	std::vector<const eve::ast::type*> arguments,
	std::vector<eve::ast::type*>& out_arguments,
	bool is_call,
	bool allow_conversions
)
{
	std::vector<eve::ast::node*> matches = context.lookup(name);

	for (size_t pass = 0; pass < 2; pass++ )
	{
		for (size_t i = 0; i < matches.size(); i++)
		{
			out_arguments.clear();

			std::map<std::string, eve::ast::argument_type*> temp_bindings = bindings;

			eve::ast::definition_variable* var = dynamic_cast<eve::ast::definition_variable*>(matches[i]);
			eve::ast::definition_function* function = dynamic_cast<eve::ast::definition_function*>(matches[i]);
			eve::ast::definition_type* type = dynamic_cast<eve::ast::definition_type*>(matches[i]);

			eve::ast::type* t = NULL;

			if (var && !is_call)
			{
				/* field access */
				if (arguments.empty())
					this->driver.debug("TODO: static field access checker::resolve()");
				
				else 
					t = var->type;

			} if (type)
			{
				if (!is_call)
				{
					/* access to a static type */
					t  = this->make_static(type->type());

				} else
				{
					/* constructor call, add the implicit self argument */
					std::vector<const eve::ast::type*> constructor_arguments;
					std::vector<eve::ast::type*> constructor_out_arguments;

					constructor_arguments.push_back(type->type()->bind(bindings));

					for (size_t a = 0; a < arguments.size(); a++)
						constructor_arguments.push_back(arguments[a]);

					t = this->resolve
					(
						"constructor",
						type->context(),
						bindings,
						constructor_arguments,
						constructor_out_arguments,
						allow_conversions					
					);

					if (t)
					{
						for (size_t a = 0; a < arguments.size(); ++a)
							out_arguments.push_back(constructor_out_arguments[a + 1]);
					}
				}

			} else if (function && is_call)
			{
				/* TODO variadic functions/variadic templates and default values */
				size_t min_args = function->function_parameters->size() - function->defaults;
				size_t max_args = function->function_parameters->size();

				std::cerr << "function: " << name << std::endl;

				if (function->variadic)
				{
					std::cerr << "variadic " << name << std::endl;
					max_args--;
					min_args--;
				}

				std::cerr << min_args << " " << arguments.size() << " " << max_args << std::endl;

				if (arguments.size() < min_args || (arguments.size() > max_args && !function->variadic))
					continue;

				bool callable = true;
				for (size_t a = 0; a < arguments.size(); a++)
				{
					std::cerr << "param:" << a << std::endl;

					/* fixme: variadic function, aditional parameters go through without type checking */
					if (function->variadic && a >= max_args)
					{
						out_arguments.push_back(arguments[a]->bind(temp_bindings));
						continue;
					}

					eve::ast::type* param = (*function->function_parameters)[a]->type()->bind(temp_bindings);

					if (allow_conversions && pass > 0)
					{
						if (!param->assignable(this->driver, arguments[a], temp_bindings))
						{
							callable = false;
							break;
						}

					} else
					{
						if (!param->equals(arguments[a], temp_bindings))
						{					
							callable = false;
							break;
						}
					}

					out_arguments.push_back(param->bind(temp_bindings));
				}

				if (callable)
				{
					bindings = temp_bindings;


					eve::target::cu cu;
					t = function->ret->bind(temp_bindings);

					return t;
				}
			}

			if (!t)
				continue;

			bindings = temp_bindings;
			return t->bind(temp_bindings);
		}
	}

	/* still havent found a match, if we are looking inside a type, check its parents */
	matches = context.lookup("eve::ast::definition_type");
	if (matches.empty())
		return NULL;

	eve::ast::definition_type* decl = dynamic_cast<eve::ast::definition_type*>(matches[0]);
	assert(decl);

	for (size_t i = 0; i < decl->parents->size(); i++)
	{
		eve::ast::type* parent = (*decl->parents)[i]->bind(bindings);
		assert(parent->base()->decl);

		eve::ast::type* value = this->resolve
		(
			name,
			parent->base()->decl->context(),
			bindings,
			arguments,
			out_arguments,
			is_call,
			allow_conversions
		);

		if (value)
			return value;
	}

	return NULL;
}

eve::ast::definition_type* checker::get_builtin(std::string name)
{
	std::string module_name = this->driver.current_path + eve::driver::BUILTIN_PATH + '/' + name + ".eve";

	std::vector<eve::ast::node*> results = this->driver.builtins[module_name]->context().lookup(name);
	assert(results.size() == 1);

	eve::ast::definition_type* builtin = dynamic_cast<eve::ast::definition_type*>(results[0]);
	assert(builtin);

	return builtin;
}

eve::ast::type* checker::make_bool()
{
	return this->get_builtin("bool")->type();
}

eve::ast::type* checker::make_shared(eve::ast::type* base)
{
	eve::ast::definition_type* shared = this->get_builtin("shared");

	std::map<std::string, eve::ast::argument_type*> bindings;
	eve::ast::argument_type_type* base_arg = new eve::ast::argument_type_type
	(
		base->location(),
		base->dup()
	);
	base_arg->_parent = base->_parent;

	bindings["T"] = base_arg;

	return shared->type()->bind(bindings);
}

eve::ast::type* checker::make_type(eve::ast::simple_id* id)
{
	std::vector<eve::ast::node*> nodes = id->context().lookup(id);
	assert(!nodes.empty());

	eve::ast::definition_type* decl = dynamic_cast<eve::ast::definition_type*>(nodes[0]);
	assert(decl);

	return decl->type();
}

eve::ast::type* checker::make_function(eve::ast::type* ret, std::vector<eve::ast::type*> parameters)
{
	eve::ast::definition_type* function = this->get_builtin("function");

	std::map<std::string, eve::ast::argument_type*> bindings;
	eve::ast::argument_type_type* ret_arg = new eve::ast::argument_type_type
	(
		ret->location(),
		ret->dup()
	);
	ret_arg->_parent = ret->_parent;

	bindings["R"] = ret_arg;

	eve::target::cu cu;

	for (size_t i = 0; i < parameters.size(); i++)
	{
		eve::ast::argument_type_type* arg_type = new eve::ast::argument_type_type
		(
			parameters[i]->location(),
			parameters[i]->dup()
		);

		arg_type->_parent = parameters[i]->_parent;

		bindings[std::to_string(i + 1)] = arg_type;
	}

	return function->type()->bind(bindings);
}

eve::ast::type* checker::make_static(eve::ast::type* type)
{	
	std::map<std::string, eve::ast::argument_type*> bindings;

	eve::ast::argument_type_type* arg_t = new eve::ast::argument_type_type
	(
		type->location(),
		type
	);
	arg_t->_parent = type->_parent;

	bindings["T"] = arg_t;
	return this->get_builtin("static")->type()->bind(bindings);
}

}
}

#endif