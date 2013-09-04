#ifndef EVE_AST_STMT_FOR_IN_IMPL
#define EVE_AST_STMT_FOR_IN_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_for_in::stmt_for_in(const eve::location& location, eve::ast::simple_id* id, eve::ast::expr* expr, std::vector<eve::ast::stmt*>* block)
	: eve::ast::stmt(location), parallel(false), id(id), expr(expr), block(block), perms(NULL), _context(NULL)
{
	this->adopt(id);
	this->adopt(expr);
	this->adopt_all(*block);
}

stmt_for_in::stmt_for_in(const eve::location& location, eve::ast::simple_id* id, eve::ast::expr* expr, std::vector<eve::ast::stmt*>* block, std::vector<eve::ast::perm*>* perms)
	: eve::ast::stmt(location), parallel(true), id(id), expr(expr), block(block), perms(perms), _context(NULL)
{
	this->adopt(id);
	this->adopt(expr);
	this->adopt_all(*block);
	this->adopt_all(*perms);
}

stmt_for_in::~stmt_for_in()
{
	delete this->id;
	delete this->expr;

	for (size_t i = 0; i < this->block->size(); i++)
		delete (*this->block)[i];

	delete this->block;

	if (this->parallel)
	{
		for (size_t i = 0; i < this->perms->size(); i++)
			delete (*this->perms)[i];

		delete this->perms;
	}
}

void stmt_for_in::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& stmt_for_in::context()
{
	return this->_context;
}


void stmt_for_in::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	driver.debug("TODO: stmt_for_in::translate()");
}

}
}

#endif