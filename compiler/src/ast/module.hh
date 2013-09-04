#ifndef EVE_AST_MODULE_IMPL
#define EVE_AST_MODULE_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

module::module(const eve::location& location, std::vector<import*>* import_list, std::vector<definition*>* decl_list)
	: eve::ast::node(location), import_list(import_list), decl_list(decl_list), _context(NULL)
{
	this->adopt_all(*import_list);
	this->adopt_all(*decl_list);
}

module::~module()
{
}

void module::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& module::context()
{
	return this->_context;
}

void module::translate(eve::driver& driver, eve::target::cu& cu)
{
	/* dont need to translate import list as it is a "preprocessor" only thing */

	for (size_t i = 0; i < this->decl_list->size(); i++)
	{
		eve::ast::definition* def = (*this->decl_list)[i];
		def->translate(driver, cu, def->top_scope(cu));
	}
}

}
}

#endif

