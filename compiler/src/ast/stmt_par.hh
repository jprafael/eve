#ifndef EVE_AST_STMT_PAR_IMPL
#define EVE_AST_STMT_PAR_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_par::stmt_par(const eve::location& location, std::vector<eve::ast::perm*>* perms, std::vector<eve::ast::stmt*>* stmts)
	: eve::ast::stmt(location), perms(perms), stmts(stmts), _context(NULL), finish(NULL), id(-1)
{
	this->adopt_all(*perms);
	this->adopt_all(*stmts);
}

stmt_par::~stmt_par()
{
	for (size_t i = 0; i < this->perms->size(); i++)
		delete (*this->perms)[i];

	delete this->perms;

	for (size_t i = 0; i < this->stmts->size(); i++)
		delete (*this->stmts)[i];

	delete this->stmts;
}

void stmt_par::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& stmt_par::context()
{
	return this->_context;
}

void stmt_par::set_finish(eve::ast::stmt_finish* finish)
{
	this->finish = finish;
	finish->par_stmts.push_back(this);
}

void stmt_par::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	std::vector<std::string> parents;
	std::vector<std::string> fields;
	std::vector<eve::target::function> constructors;
	std::vector<eve::target::function> methods;

	eve::target::task task;
	eve::target::function constructor;
	eve::target::function execute;
	eve::target::function exec;
	
	if (!parallel)
	{
		for (size_t i = 0; i < this->stmts->size(); i++)
			(*this->stmts)[i]->translate(driver, cu, scope, false);

		return;
	}

	task.id = std::string("eve_par_") + std::to_string(this->id);
	task.parents.push_back("eve::rt::task");

	constructor.type = task.id;

	if (this->finish)
	{
		std::string name = std::string("finish_") + std::to_string(this->finish->id);
		std::string arg;

		if (this->finish->single_task())
			arg = std::string("volatile bool& ") + name;

		else
			arg = std::string("volatile int& ") + name;

		task.fields.push_back(arg);
		constructor.parameters.push_back(arg);
		constructor.assigns.push_back(name + "(" + name + ")");
	}

	exec.type = "void";
	exec.name = "__exec";

	for (size_t i = 0; i < this->stmts->size(); i++)
		(*this->stmts)[i]->translate(driver, cu, exec.stmts, parallel);

	if (this->finish)
	{
		if (this->finish->single_task())
			task.post_exec.push_back(std::string("finish_") + std::to_string(this->finish->id) + " = true;\n");

		else
			task.post_exec.push_back(std::string("__sync_sub_and_fetch(&finish_") + std::to_string(this->finish->id) + ", 1);\n");

	} else if (!this->finish)
		task.post_exec.push_back("delete this;\n");

	task.constructors.push_back(constructor);

	execute.type = "void";
	execute.name = "operator()";
	execute.stmts.push_back("__pre_exec();");
	execute.stmts.push_back("__exec();");
	execute.stmts.push_back("__post_exec();");

	task.methods.push_back(exec);
	task.methods.push_back(execute);

	/* create and schedule the task */
	std::string create;
	if (this->finish)
		create = task.id + " _" + task.id;

	else
		create = task.id + "* _" + task.id +" = new " + task.id;

	std::vector<std::string> arguments;
	if (this->finish)
		arguments.push_back(std::string("finish_") + std::to_string(this->finish->id));

	for (size_t i = 0; i < this->perms->size(); i++)
		(*this->perms)[i]->translate(driver, cu, task, arguments, this->finish);

	if (!arguments.empty())
	{
		create += "(";

		for (size_t i = 0; i < arguments.size(); i++)
		{
			create += arguments[i];

			if (i != arguments.size() - 1)
				create += ", ";
		}

		create += ")";
	}

	create += ";";

	std::string schedule = "eve::rt::worker::current()->schedule(";
	if (!this->finish)
		schedule += "*";

	schedule += "_" + task.id + ");\n";

	scope.push_back(create);
	scope.push_back(schedule);

	cu.type_forwards.push_back("class " + task.id + ";\n");
	cu.type_declarations.push_back(task.string());

	if (this->finish && !this->finish->single_task())
		scope.push_back(std::string("__sync_add_and_fetch(&finish_") + std::to_string(this->finish->id) + ", 1);\n");
}

}
}

#endif