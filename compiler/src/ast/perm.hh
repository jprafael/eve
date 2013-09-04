#ifndef EVE_AST_THIS_IMPL
#define EVE_AST_THIS_IMPL

#include "../ast/ast.hh"
#include "../visitor/visitor.hh"
#include "../target/target.hh"

namespace eve
{
namespace ast
{

perm::perm(const eve::location& location, eve::ast::id* id, eve::ast::perm_access access)
	: eve::ast::node(location), id(id), access(access)
{
	this->adopt(id);
	this->shared =
		this->access == eve::ast::perm_access::access_full ||
		this->access == eve::ast::perm_access::access_none ||
		this->access == eve::ast::perm_access::access_const;
}

perm::~perm()
{
	delete this->id;
}

void perm::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

void perm::translate(eve::driver& driver, eve::target::cu& cu, eve::target::task& task, std::vector<std::string>& arguments, eve::ast::stmt_finish* finish)
{
	std::vector<eve::ast::node*> nodes = this->context().lookup(this->id);

	assert(!nodes.empty());
	eve::ast::node* node = nodes[0];

	std::string name = std::string("eve_") + this->id->string();

	assert(node);
	
	eve::ast::type* type;	

	eve::ast::definition_variable* var = dynamic_cast<eve::ast::definition_variable*>(node);
	eve::ast::parameter_function* param = dynamic_cast<eve::ast::parameter_function*>(node);

	// FIXME: for_each constructor
	if (var)
		type = var->type;
	else if (param)
		type = param->type();
	else
		driver.debug(this->location(), "TODO: this is not a variable nor a function parameter");

	if (this->access == eve::ast::perm_access::access_ref)
		task.fields.push_back(type->translate(driver, cu) + "& " + name);
	else
		task.fields.push_back(type->translate(driver, cu) + " " + name);

	if (shared)
		task.fields.push_back(std::string("eve::rt::monitor& ") + name + "_monitor");

	for (size_t i = 0; i < task.constructors.size(); i++)
	{
		std::string param;
		if (this->access == eve::ast::perm_access::access_copy)
			param += "const ";

		param += type->translate(driver, cu) + "& " + name;

		task.constructors[i].parameters.push_back(param);
		task.constructors[i].assigns.push_back(name + "(" + name + ")");

		if (this->shared)
		{
			task.constructors[i].parameters.push_back(std::string("eve::rt::monitor& ") + name +"_monitor");
			task.constructors[i].assigns.push_back(name + "_monitor(" + name + "_monitor)");
		}
	}

	if (this->access == eve::ast::perm_access::access_const)
	{
		task.pre_exec.push_back(name +  "_monitor.lock_shared();");
		task.post_exec.push_front(name +  "_monitor.unlock_shared();");

	} else if (this->access == eve::ast::perm_access::access_full)
	{
		task.pre_exec.push_back(name +  "_monitor.lock_unique();");
		task.post_exec.push_front(name +  "_monitor.unlock_unique();");
	}

	arguments.push_back(name);

	if (this->shared)
	{
		if (finish)
			arguments.push_back(name + "_monitor_" + std::to_string(finish->id));

		else
			arguments.push_back(name + ".monitor()");
	}
}

}
}

#endif
