#ifndef EVE_AST_DEFINITION_FUNCTION_NAMED_EXPLICIT_IMPL
#define EVE_AST_DEFINITION_FUNCTION_NAMED_EXPLICIT_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{


definition_function_named_explicit::definition_function_named_explicit(const eve::location& location, eve::ast::type* _type, std::vector<parameter_type*>* template_parameters, std::vector<parameter_function*>* function_parameters, std::vector<perm*>* perms, eve::ast::simple_id* id, std::vector<stmt*>* stmts)
	: eve::ast::definition_function_named(location, _type, template_parameters, function_parameters, perms, id), stmts(stmts)
{
	this->adopt_all(*this->stmts);
}

definition_function_named_explicit::~definition_function_named_explicit()
{
	for (size_t i = 0; i < this->stmts->size(); i++)
		delete (*this->stmts)[i];

	delete this->stmts;
}

void definition_function_named_explicit::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

bool definition_function_named_explicit::is_main()
{
	if (this->id->string() != "main")
		return false;

	/* TODO: resolve() types */
	/*
	if (this->type->resolve() != this->context().lookup("void"))
		return false;
	*/

	/* TODO: pass arguments to main task */
	if (this->function_parameters->size() != 0)
		return false;

	return true;
}

void definition_function_named_explicit::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope)
{
	if (this->builtin)
		return;

	eve::target::function fn;
	fn.type = this->ret->translate(driver, cu);
	fn.name = "eve_" + this->id->string();

	/* TODO: is this valid for templated functions? */
	std::string forward = fn.type + " " + fn.name + "(";

	/* parameters */
	for (size_t i = 0; i < this->function_parameters->size(); i++)
	{
		eve::ast::parameter_function* p = (*this->function_parameters)[i];

		/* c++ doesnt need self as an argument */
		if (dynamic_cast<eve::ast::parameter_function_self*>(p))
			continue;

		eve::ast::parameter_function_named* named = dynamic_cast<eve::ast::parameter_function_named*>(p);
		assert(named);

		std::string param = named->type()->translate(driver, cu) + " eve_" + named->id->string();
		fn.parameters.push_back(param);
		forward += param;

		if (i != this->function_parameters->size() - 1)
			forward += ", ";
	}

	forward += ");\n";

	/* statements */
	for (size_t i = 0; i < this->stmts->size(); i++)
	{
		eve::ast::stmt* s = (*this->stmts)[i];
		s->translate(driver, cu, fn.stmts, true);
	}

	cu.function_forwards.push_back(forward);
	cu.function_declarations.push_back(fn.string());

	if (!this->is_main())
		return;

	eve::target::task task;

	/* fill in the task declaration */
	task.id = fn.name + "_task";
	task.parents.push_back("eve::rt::task");

	/* constructor */
	eve::target::function constructor;
	constructor.type = task.id;

	for (size_t i = 0; i < this->function_parameters->size(); i++)
	{
		eve::ast::parameter_function* p = (*this->function_parameters)[i];

		if (dynamic_cast<eve::ast::parameter_function_self*>(p))
			continue;

		eve::ast::parameter_function_named* named = dynamic_cast<eve::ast::parameter_function_named*>(p);
		assert(named);

		task.fields.push_back(named->type()->translate(driver, cu) + " " + named->id->string());
		constructor.parameters.push_back(named->type()->translate(driver, cu) + " " + named->id->string());
		constructor.stmts.push_back("this->" + named->id->string() + " = " + named->id->string() + ";");
	}

	eve::target::function execute;
	execute.type = "void";
	execute.name = "operator()";
	execute.stmts.push_back("__pre_exec();");
	execute.stmts.push_back(fn.name + "();");
	execute.stmts.push_back("__post_exec();");

	task.constructors.push_back(constructor);
	task.methods.push_back(execute);

	/* TODO: handle perms */
	if (this->is_main())
	{
		std::vector<std::string> arguments;

		/* if it is a main task */
		for (size_t i = 0; i < this->perms->size(); i++)
			(*this->perms)[i]->translate(driver, cu, task, arguments, NULL);

		std::string main_decl;

		main_decl += "int main(int argc, char** argv)\n";
		main_decl += "{\n";
		main_decl += "	eve::rt::runtime rt;\n";
		main_decl += "	__init(rt);\n";

		main_decl += "	" + task.id + " main";
		if (!arguments.empty())
		{
			main_decl += "(";

			for (size_t i = 0; i < arguments.size(); i++)
			{
				main_decl += arguments[i];
				if (i != arguments.size() - 1)
					main_decl += ", ";
			}

			main_decl += ")";
		}

		main_decl += ";\n";
		main_decl += "	rt[0].schedule(main);\n";
		main_decl += "\n";
		main_decl += "	rt.startall();\n";
		main_decl += "	rt.waitall();\n";
		main_decl += "\n";
		main_decl += "	return 0;\n";
		main_decl += "}\n";

		cu.main_declaration.push_back(main_decl);
	}

	cu.type_forwards.push_back("class " + task.id + ";\n");
	cu.type_declarations.push_back(task.string());
}

}
}

#endif
