#ifndef EVE_AST_STMT_IF_IMPL
#define EVE_AST_STMT_IF_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_if::stmt_if(const eve::location& location, eve::ast::expr* expr, std::vector<eve::ast::stmt*>* then_block, std::vector<eve::ast::stmt*>* else_block)
	: eve::ast::stmt(location), expr(expr), then_block(then_block), else_block(else_block), _context(NULL)
{
	this->adopt(expr);
	this->adopt_all(*then_block);

	if (else_block)
		this->adopt_all(*else_block);
}

stmt_if::~stmt_if()
{
	delete this->expr;

	for (size_t i = 0; i < this->then_block->size(); i++)
		delete (*this->then_block)[i];

	delete this->then_block;

	if (this->else_block)
	{
		for (size_t i = 0; i < this->else_block->size(); i++)
			delete (*this->else_block)[i];

		delete this->else_block;		
	}
}

void stmt_if::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& stmt_if::context()
{
	return this->_context;
}

void stmt_if::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	std::string stmt = "if (" + this->expr->translate(driver, cu) + ")\n";

	std::vector<std::string> then_scope;

	stmt += "{\n";

	for (size_t i = 0; i < this->then_block->size(); i++)
		(*this->then_block)[i]->translate(driver, cu, then_scope, parallel);

	for (size_t i = 0; i < then_scope.size(); i++)
		stmt += then_scope[i] + "\n";

	stmt += "}";

	if (this->else_block)
	{
		std::vector<std::string> else_scope;

		stmt += " else\n";
		stmt += "{\n";

		for (size_t i = 0; i < this->else_block->size(); i++)
			(*this->else_block)[i]->translate(driver, cu, else_scope, parallel);

		for (size_t i = 0; i < else_scope.size(); i++)
			stmt += else_scope[i] + "\n";
		
		stmt += "}";
	}

	stmt += "\n";
	scope.push_back(stmt);
}

}
}

#endif