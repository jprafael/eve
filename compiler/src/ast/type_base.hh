#ifndef EVE_AST_TYPE_BASE_IMPL
#define EVE_AST_TYPE_BASE_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

type_base::type_base(const eve::location& location, eve::ast::id* id, std::vector<eve::ast::argument_type*>* arguments)
	: eve::ast::type(location), id(id), arguments(arguments), decl(NULL)
{
	this->adopt(id);
	this->adopt_all(*arguments);
}

type_base::~type_base()
{
	delete this->id;

	for (size_t i = 0; i < this->arguments->size(); i++)
		delete (*this->arguments)[i];

	delete this->arguments;
}

void type_base::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string type_base::translate(eve::driver& driver, eve::target::cu& cu)
{
/*	eve::ast::definition_type* def = dynamic_cast<eve::ast::definition_type*>(this->context().lookup(this->id));
	assert(def);

	return def->translate_type(driver, *this);
*/	
	std::string r = "eve_" + this->id->string();
	if (this->arguments->size() == 0)
		return r;

	/* templates: replace int<32> with int_32 */
	r += "<";

	for (size_t i = 0; i < this->arguments->size(); i++)
	{
		r += (*this->arguments)[i]->translate(driver, cu);

		if (i != this->arguments->size() - 1)
			r += ", ";
	}

	r += ">";

	return r;
}

eve::ast::type* type_base::dup() const
{
	std::vector<eve::ast::argument_type*>* arguments = new std::vector<eve::ast::argument_type*>();

	for (size_t i = 0; i < this->arguments->size(); i++)
		arguments->push_back((*this->arguments)[i]->dup());

	eve::ast::type_base* base = new eve::ast::type_base(this->location(), this->id->dup(), arguments);
	base->decl = this->decl;
	base->_parent = this->_parent;

	return base;
}

eve::ast::type_base* type_base::base() const
{
	return static_cast<eve::ast::type_base*>(this->dup());
}

eve::ast::type* type_base::bind(std::map<std::string, eve::ast::argument_type*> bindings) const
{
	std::map<std::string, eve::ast::argument_type*>::iterator it;
	it = bindings.find(this->id->string());

	if (it != bindings.end())
	{
		assert(this->arguments->size() == 0);

		eve::ast::argument_type_type* arg = dynamic_cast<eve::ast::argument_type_type*>(it->second);
		assert(arg);

		return arg->type;
	}

	std::vector<eve::ast::argument_type*>* arguments = new std::vector<eve::ast::argument_type*>();
	for (size_t i = 0; i < this->arguments->size(); i++)
	{
		eve::ast::argument_type* arg = (*this->arguments)[i];
		eve::ast::argument_type_id* arg_id = dynamic_cast<eve::ast::argument_type_id*>((*this->arguments)[i]);

		if (!arg_id)
		{
 			arguments->push_back(arg->bind(bindings));
 			continue;
		}

		/* id can be a variadic parameter */
		std::vector<eve::ast::node*> params = this->context().lookup(arg_id->id);
		if (params.empty())
		{
 			arguments->push_back(arg->bind(bindings));
 			continue;
 		}

		eve::ast::parameter_type_type* param_type = dynamic_cast<eve::ast::parameter_type_type*>(params[0]); 
		eve::ast::parameter_type_constant* param_constant = dynamic_cast<eve::ast::parameter_type_constant*>(params[0]); 

		if ((param_type && param_type->elipsis) || (param_constant && param_constant->elipsis))
		{
			assert(i == this->arguments->size() - 1);

			while (true)
			{
				auto it = bindings.find(std::to_string(i));
				if (it == bindings.end())
					break;

				arguments->push_back(it->second);
				++i;
			}

		} else
			arguments->push_back(arg->bind(bindings));
	}

	eve::ast::type_base* t = new eve::ast::type_base(
		this->location(),
		this->id->dup(),
		arguments
	);

	t->decl = this->decl;
	t->_parent = this->_parent;

	return t;
}

bool type_base::is(const std::string& id) const
{
	return this->id->string() == id;
}

bool type_base::equals(const eve::ast::type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const
{
	if (this == rhs)
		return true;

	const eve::ast::type_base* base = dynamic_cast<const eve::ast::type_base*>(rhs);
	if (!base)
		return false;

	if (this->decl != base->decl)
	{
		if (!dynamic_cast<eve::ast::parameter_type_type*>(this->decl))
			return false;

		/* only relaces T with int, and not T<A,B> with int<A,B> so far */
		assert(this->arguments->empty());

		/* binding an unbound template type */
		out_bindings[this->id->string()] = new eve::ast::argument_type_type
		(
			rhs->location(),
			rhs->dup()
		);
	}

	eve::ast::definition_type* decl = dynamic_cast<eve::ast::definition_type*>(base->decl);
	assert(decl);

	for (size_t i = 0; i < decl->parameters->size(); i++)
	{
		eve::ast::argument_type* left;
		eve::ast::argument_type* right;
		eve::ast::argument_type* default_arg = NULL;

		eve::ast::parameter_type* param = (*decl->parameters)[i];
		eve::ast::parameter_type_type* param_type = dynamic_cast<eve::ast::parameter_type_type*>(param);
		eve::ast::parameter_type_constant* param_constant = dynamic_cast<eve::ast::parameter_type_constant*>(param);

		if ((param_type && param_type->elipsis) || (param_constant && param_constant->elipsis))
		{
			for (; i < base->arguments->size(); i++)
				out_bindings[std::to_string(i)] = (*base->arguments)[i];

			return true;
		}

		if (param_type)
		{
			if (param_type->type)
			{
				default_arg = new eve::ast::argument_type_type
				(
					this->location(),
					param_type->type->dup()
				);
			}

		} else if (param_constant)
		{
			if (param_constant->constant)
			{
				default_arg = new eve::ast::argument_type_constant
				(
					this->location(),
					param_constant->constant->dup()
				);
			}
		} else
			assert(false);

		if (i < this->arguments->size())
			left = (*this->arguments)[i];
		else
			left = default_arg;

		if (i < base->arguments->size())
			right = (*base->arguments)[i];
		else
			right = default_arg;
		
		/* not enough arguments */
		if (!left || !right)
			return false;

		if (!left->equals(right, out_bindings))
			return false;
	}

	return true;
}

bool type_base::assignable(eve::driver& driver, const eve::ast::type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const
{
	if (this->equals(rhs, out_bindings))
		return true;

	const eve::ast::type_base* rhsb = dynamic_cast<const eve::ast::type_base*>(rhs);
		
	if (!rhsb)
		return false;

	eve::target::cu cu;

	std::vector<const eve::ast::type*> arguments;
	std::vector<eve::ast::type*> out_arguments;

	arguments.push_back(this);
	arguments.push_back(rhs);

	/* litle hack for shared objects */
	if (!this->is("shared") && rhsb->is("shared"))
	{
		/* check for T = shared<T> */
		const eve::ast::argument_type_type* rhs_base = dynamic_cast<const eve::ast::argument_type_type*>(rhsb->bindings()["T"]);
		assert(rhs_base);

		return this->assignable(driver, rhs_base->type, out_bindings);

	} /* downcasting is handled normaly trough shared<T>::operator = (const T& o) */

	/* check if rhs is a parent of this */
	eve::ast::definition_type* rhsb_decl = dynamic_cast<eve::ast::definition_type*>(rhsb->decl);
	if (rhsb_decl)
	{
		for (size_t i = 0; i < rhsb_decl->parents->size(); i++)
		{
			eve::ast::type* parent = (*rhsb_decl->parents)[i]->bind(rhsb->bindings());

			if (this->equals(parent, out_bindings))
				return true;
		}

	}

	/* check if conversion exists */
	eve::ast::type* ret = driver.checker().resolve
	(
		std::string("constructor"),
		this->decl->context(),
		this->bindings(),
		arguments,
		out_arguments,
		true,
		false
	);

	return ret != NULL;
}

std::map<std::string, eve::ast::argument_type*> type_base::bindings() const
{
	std::map<std::string, eve::ast::argument_type*> bindings;

	assert(this->decl);
	eve::ast::definition_type* decl = dynamic_cast<eve::ast::definition_type*>(this->decl);

	if (!decl)
		return bindings;

	eve::driver d("");
	eve::target::cu cu;
	std::cerr << const_cast<eve::ast::type_base*>(this)->translate(d, cu) << std::endl;

	assert(decl); /* TODO: type specialization */

	for (size_t i = 0; i < decl->parameters->size(); i++)
	{
		eve::ast::parameter_type* param = (*decl->parameters)[i];
		eve::ast::parameter_type_type* type = dynamic_cast<eve::ast::parameter_type_type*>(param);
		eve::ast::parameter_type_constant* constant = dynamic_cast<eve::ast::parameter_type_constant*>(param);

		eve::ast::simple_id* id;

		assert(type || constant);

		if (type)
			id = type->id;
		else
			id = constant->id;

		if ((type && type->elipsis) || (constant && constant->elipsis))
		{
			for (; i < this->arguments->size(); i++)
				bindings[std::to_string(i)] = (*this->arguments)[i];

			return bindings;
		}

		if (i < this->arguments->size())
		{
			eve::ast::argument_type_id* type_id = dynamic_cast<eve::ast::argument_type_id*>((*this->arguments)[i]);

			if (!type_id)
			{
				bindings[id->string()] = (*this->arguments)[i];

			} else
			{
				std::vector<eve::ast::node*> results = decl->context().lookup(type_id->id);
				std::cerr<< type_id->id->string() << " " << results.size();
				
				if (results.empty())
					continue;

				eve::ast::definition_type* def_type = dynamic_cast<eve::ast::definition_type*>(results[i]);
				eve::ast::definition_variable* def_variable = dynamic_cast<eve::ast::definition_variable*>(results[i]);

				if (def_type)
				{
					eve::ast::argument_type_type* arg = new eve::ast::argument_type_type(
						this->location(),
						def_type->type()
					);

					arg->_parent = this->_parent;

					bindings[id->string()] = arg;
				} else if (def_variable)
				{
					/* TODO: constant values as type arguments */
					assert(false);

				} else
				{
					/* int<B>, B is still unbound, no need to add to bindings */
					assert(dynamic_cast<eve::ast::parameter_type*>(results[i]));
				}
			}
		} else
		{
			if (type)
			{
				eve::ast::argument_type_type* arg = new eve::ast::argument_type_type
				(
					this->location(),
					type->type->dup()
				);

				arg->_parent = this->_parent;
				bindings[id->string()] = arg;

			} else
			{
				eve::ast::argument_type_constant* arg = new eve::ast::argument_type_constant
				(
					this->location(),
					constant->constant->dup()
				);

				arg->_parent = this->_parent;
				bindings[id->string()] = arg;
			}
		}
	}

	return bindings;
}

bool type_base::requires_conversion(const eve::ast::type* rhs) const
{
	std::map<std::string, eve::ast::argument_type*> bindings;

	return !this->equals(rhs, bindings);
}

}
}

#endif