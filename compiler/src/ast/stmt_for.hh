#ifndef EVE_AST_STMT_FOR_IMPL
#define EVE_AST_STMT_FOR_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

stmt_for::stmt_for(const eve::location& location, eve::ast::expr* init, eve::ast::expr* cond, eve::ast::expr* incr, std::vector<eve::ast::stmt*>* block)
	: eve::ast::stmt(location), parallel(false), init(init), cond(cond), incr(incr), block(block), perms(NULL), _context(NULL)
{
	this->adopt(init);
	this->adopt(cond);
	this->adopt(incr);
	this->adopt_all(*block);
}

stmt_for::stmt_for(const eve::location& location, eve::ast::expr* init, eve::ast::expr* cond, eve::ast::expr* incr, std::vector<eve::ast::stmt*>* block, std::vector<eve::ast::perm*>* perms)
	: eve::ast::stmt(location), parallel(true), init(init), cond(cond), incr(incr), block(block), perms(perms), _context(NULL)
{
	this->adopt(init);
	this->adopt(cond);
	this->adopt(incr);
	this->adopt_all(*block);
	this->adopt_all(*perms);
}

stmt_for::~stmt_for()
{
	if (this->init)
		delete this->init;

	if (this->cond)
		delete this->cond;

	if (this->incr)
		delete this->incr;

	for (size_t i = 0; i < this->block->size(); i++)
		delete (*this->block)[i];

	delete this->block;

	if (this->parallel)
	{
		for (size_t i = 0; i < this->block->size(); i++)
			delete (*this->block)[i];

		delete this->block;
	}
}

void stmt_for::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& stmt_for::context()
{
	return this->_context;
}

void stmt_for::translate(eve::driver& driver, eve::target::cu& cu, std::vector<std::string>& scope, bool parallel)
{
	std::string stmt = "for(";

	if (this->init)
		stmt += this->init->translate(driver, cu);

	stmt += "; ";

	if (this->cond)
		stmt += this->cond->translate(driver, cu);

	stmt += "; ";

	if (this->incr)
		stmt += this->incr->translate(driver, cu);

	stmt += ")\n{";

	if (this->parallel && parallel)
		driver.debug("TODO: stmt_for::translate() with parallel");

	std::vector<std::string> block;

	for (size_t i = 0; i < this->block->size(); i++)
		(*this->block)[i]->translate(driver, cu, block, parallel);

	for (size_t i = 0; i < block.size(); i++)
		stmt += block[i];

	stmt += "}\n";

	scope.push_back(stmt);
}

}
}

#endif