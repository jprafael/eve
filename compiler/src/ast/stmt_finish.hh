#ifndef EVE_AST_STMT_FINISH_IMPL
#define EVE_AST_STMT_FINISH_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_finish::stmt_finish(const eve::location& location, std::vector<eve::ast::stmt*>* stmts)
	: eve::ast::stmt(location), stmts(stmts), _context(NULL), id(-1)
{
	this->adopt_all(*stmts);
}

stmt_finish::~stmt_finish()
{
	for (size_t i = 0; i < this->stmts->size(); i++)
		delete (*this->stmts)[i];

	delete this->stmts;
}

void stmt_finish::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& stmt_finish::context()
{
	return this->_context;
}

void stmt_finish::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	if (!parallel)
	{
		for (size_t i = 0; i < this->stmts->size(); i++)
			(*this->stmts)[i]->translate(driver, cu, scope, false);

		return;
	}

	std::string if_stmt = "if (eve::rt::worker::current()->parallelize())\n{\n";

	std::vector<std::string> then_block;
	std::vector<std::string> else_block;

	/* parallel */

	if (this->single_task())
		then_block.push_back(std::string("volatile bool finish_") + std::to_string(this->id) + " = false;\n");
	else
		then_block.push_back(std::string("volatile int finish_") + std::to_string(this->id) + " = 0;\n");

	for (std::set<std::string>::iterator it = this->monitors.begin();
		it != this->monitors.end();
		++it)
	{
		then_block.push_back(std::string("eve::rt::monitor eve_") + (*it) + "_monitor_" + std::to_string(this->id) +";\n");
	}

	for (size_t i = 0; i < this->stmts->size(); i++)
		(*this->stmts)[i]->translate(driver, cu, then_block, true);

	if (this->single_task())
		then_block.push_back(std::string("eve::rt::worker::current()->complete(finish_") + std::to_string(this->id) + ");\n");

	/* sequential */
	for (size_t i = 0; i < this->stmts->size(); i++)
		(*this->stmts)[i]->translate(driver, cu, else_block, false);

	for (size_t i = 0; i < then_block.size(); i++)
		if_stmt += then_block[i] + "\n";

	if_stmt += "\n} else\n{\n";
	for (size_t i = 0; i < else_block.size(); i++)
		if_stmt += else_block[i] + "\n";

	if_stmt += "\n}";

	scope.push_back(if_stmt);
}

bool stmt_finish::single_task()
{
	return this->par_stmts.size() == 1 && this->par_stmts[0]->_parent == this;
}

}
}

#endif
