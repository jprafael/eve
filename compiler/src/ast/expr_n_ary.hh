#ifndef EVE_AST_EXPR_N_ARY_IMPL
#define EVE_AST_EXPR_N_ARY_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_n_ary::expr_n_ary(const eve::location& location, eve::ast::expr* base, eve::ast::op op, std::vector<eve::ast::expr*>* arguments, std::vector<eve::ast::argument_function_named*>* named_arguments)
	: eve::ast::expr(location), base(base), op(op), arguments(arguments), named_arguments(named_arguments)
{
	this->adopt(base);
	this->adopt_all(*arguments);
	this->adopt_all(*named_arguments);
}

expr_n_ary::~expr_n_ary()
{
	delete this->base;

	for (size_t i = 0; i < this->arguments->size(); i++)
		delete (*this->arguments)[i];

	delete this->arguments;

	for (size_t i = 0; i < this->named_arguments->size(); i++)
		delete (*this->named_arguments)[i];

	delete this->named_arguments;
}

void expr_n_ary::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string expr_n_ary::translate(eve::driver& driver, eve::target::cu& cu)
{
	std::string ret;

	if (this->base)
	{
		ret += this->base->translate(driver, cu);

		if (this->base->type()->base()->is("static"))
			ret += "::";

		else
			ret += ".";
	} else
		ret += "eve_";

	ret += this->id->string();

	if (op == n_ary_parens)
		ret += "(";

	else
		ret += "[";

	size_t j = 0;

	for (size_t i = 0; i < this->arguments->size(); i++, j++)
	{
		ret += (*this->arguments)[i]->translate(driver, cu);
	
		if (i != this->arguments->size() - 1 || this->named_arguments->size() > 0)
			ret += ", ";
	}

	if (this->named_arguments->size() > 0)
	{
		for (size_t i = 0; i < this->named_arguments->size(); i++, j++)
		{
			ret += (*this->named_arguments)[i]->expr->translate(driver, cu);

			if (i != this->named_arguments->size() - 1)
				ret += ", ";
		}
	}

	if (op == n_ary_parens)
		ret += ")";
	else
		ret += "]";

	return this->wrap(ret, driver, cu);
}

}
}

#endif
