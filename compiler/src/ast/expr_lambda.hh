#ifndef EVE_AST_EXPR_LAMBDA_IMPL
#define EVE_AST_EXPR_LAMBDA_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

expr_lambda::expr_lambda(const eve::location& location, eve::ast::type* ret, std::vector<eve::ast::parameter_function*>* parameters, std::vector<eve::ast::perm*>* perms, eve::ast::expr* expr)
	: eve::ast::expr(location), _ret(ret), parameters(parameters), perms(perms), _context(NULL), id(-1)
{
	this->stmts = new std::vector<eve::ast::stmt*>();
	this->stmts->push_back(new eve::ast::stmt_return(expr->location(), expr));

	this->adopt(ret);
	this->adopt_all(*parameters);
	this->adopt_all(*perms);
	this->adopt_all(*this->stmts);
}

expr_lambda::expr_lambda(const eve::location& location, eve::ast::type* ret, std::vector<eve::ast::parameter_function*>* parameters, std::vector<eve::ast::perm*>* perms, std::vector<eve::ast::stmt*>* stmts)
	: eve::ast::expr(location), _ret(ret), parameters(parameters), perms(perms), stmts(stmts), _context(NULL), id(-1)
{
	this->adopt(ret);
	this->adopt_all(*parameters);
	this->adopt_all(*perms);
	this->adopt_all(*stmts);
}

expr_lambda::~expr_lambda()
{
	delete this->_type;

	for (size_t i = 0; i < this->parameters->size(); i++)
		delete (*this->parameters)[i];

	delete this->parameters;

	for (size_t i = 0; i < this->perms->size(); i++)
		delete (*this->perms)[i];

	delete this->perms;

	for (size_t i = 0; i < this->stmts->size(); i++)
		delete (*this->stmts)[i];

	delete this->stmts;
}

void expr_lambda::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& expr_lambda::context()
{
	return this->_context;
}

std::string expr_lambda::translate(eve::driver& driver, eve::target::cu& cu)
{
	std::vector<std::string> parents;
	std::vector<eve::target::function> constructors;
	std::vector<eve::target::function> methods;

	eve::target::task task;
	eve::target::function constructor;
	eve::target::function execute;

	bool shared = false;
	if (this->_expected && this->_expected->is("shared"))
		shared = true;

	task.id = std::string("eve_lambda_") + std::to_string(this->id);

	execute.type = this->_ret->translate(driver, cu);
	execute.name = "operator()";
	
	constructor.type = task.id;

	std::string parent = "eve::rt::function<" + execute.type;

	for (size_t i = 0; i < this->parameters->size(); i++)
	{
		eve::ast::parameter_function_named* param = dynamic_cast<eve::ast::parameter_function_named*>((*this->parameters)[i]);
		assert(param);

		parent += ", ";
		parent += param->type()->translate(driver, cu);

		execute.parameters.push_back(param->type()->translate(driver, cu) + " eve_" + param->id->string());
	}

	parent += ">";
	task.parents.push_back(parent);

	for (size_t i = 0; i < this->stmts->size(); i++)
		(*this->stmts)[i]->translate(driver, cu, execute.stmts, true);

	task.constructors.push_back(constructor);
	task.methods.push_back(execute);

	std::vector<std::string> arguments;
	
	for (size_t i = 0; i < this->perms->size(); i++)
		(*this->perms)[i]->translate(driver, cu, task, arguments, NULL);

	std::string create;

	if (shared)
		create = "eve::rt::make_shared<" + task.id + ">(";
	else
		create = task.id + "(";

	if (!arguments.empty())
	{
		for (size_t i = 0; i < arguments.size(); i++)
		{
			create += arguments[i];

			if (i != arguments.size() - 1)
				create += ", ";
		}
	}

	create += ")";

	cu.type_forwards.push_back("class " + task.id + ";\n");
	cu.type_declarations.push_back(task.string());

	return create; /* this->wrap(create)*/
}

}
}

#endif
