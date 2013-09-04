#ifndef EVE_AST_STMT_WHILE_IMPL
#define EVE_AST_STMT_WHILE_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_while::stmt_while(const eve::location& location, eve::ast::expr* expr, std::vector<eve::ast::stmt*>* block)
	: eve::ast::stmt(location), expr(expr), block(block), _context(NULL)
{
	this->adopt(expr);
	this->adopt_all(*block);
}

stmt_while::~stmt_while()
{
	delete this->expr;

	for (size_t i = 0; i < this->block->size(); i++)
		delete (*this->block)[i];

	delete this->block;
}

void stmt_while::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& stmt_while::context()
{
	return this->_context;
}

void stmt_while::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	std::string stmt = "while (" + this->expr->translate(driver, cu) + ")\n";
	std::vector<std::string> block_scope;

	stmt += "{\n";

	for (size_t i = 0; i < this->block->size(); i++)
		(*this->block)[i]->translate(driver, cu, block_scope, parallel);

	for (size_t i = 0; i < block_scope.size(); i++)
		stmt += block_scope[i] + "\n";

	stmt += "}\n\n";

	scope.push_back(stmt);
}

}
}

#endif