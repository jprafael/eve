#ifndef EVE_AST_SIMPLE_ID_IMPL
#define EVE_AST_SIMPLE_ID_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

simple_id::simple_id(const eve::location& location, std::string* id)
	: eve::ast::id(location), id(id)
{
}

simple_id::simple_id(std::string* id)
	: eve::ast::id(eve::location()), id(id)
{
}

simple_id::~simple_id()
{
	delete this->id;
}

void simple_id::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

const eve::ast::simple_id* simple_id::simple() const
{
	return this;
}

const std::vector<eve::ast::simple_id*> simple_id::ids() const
{
	std::vector<eve::ast::simple_id*> v;
	v.push_back(const_cast<eve::ast::simple_id*>(this));
	return v;
}

std::string simple_id::string() const
{
	return *this->id;
}

eve::ast::simple_id* simple_id::dup() const
{
	eve::ast::simple_id* id = new eve::ast::simple_id(this->location(), new std::string(*this->id));
	id->_parent = this->_parent;
	return id;
}

}
}

#endif