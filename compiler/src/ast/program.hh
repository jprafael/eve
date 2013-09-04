#ifndef EVE_AST_PROGRAM_IMPL
#define EVE_AST_PROGRAM_IMPL

#include "../driver.hh"
#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

program::program()
	: eve::ast::node(eve::location()), _context(NULL)
{
}

program::~program()
{
}

void program::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::symbol::context& program::context()
{
	return this->_context;
}

void program::translate(eve::driver& driver, eve::target::cu& cu)
{
	cu.imports.push_back("#include \"runtime/eve/eve.hpp\"\n");
	cu.imports.push_back("#include \"runtime/builtin.hpp\"\n");

	cu.imports.push_back("using namespace eve;");

	for (module_map_t::iterator it = this->modules.begin(); it != this->modules.end(); ++it)
		(*it).second->translate(driver, cu);

}

void program::add_module(const std::string& filename, eve::ast::module* module)
{
	this->modules[filename] = module;

	this->context().insert(filename, module);
	this->adopt(module);
}

}
}

#endif