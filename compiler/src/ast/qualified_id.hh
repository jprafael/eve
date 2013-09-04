#ifndef EVE_AST_QUALIFIED_ID_IMPL
#define EVE_AST_QUALIFIED_ID_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

qualified_id::qualified_id(const eve::location& location, eve::ast::simple_id* left, eve::ast::simple_id* right)
	: eve::ast::id(location)
{
	this->_ids.push_back(left);
	this->_ids.push_back(right);

	this->adopt(left);
	this->adopt(right);
}

qualified_id::qualified_id(const eve::location& location, std::vector<eve::ast::simple_id*> ids)
	: eve::ast::id(location), _ids(ids)
{
	this->adopt_all(_ids);
}

qualified_id::~qualified_id()
{
	for (size_t i = 0; i < this->_ids.size(); i++)
		delete this->_ids[i];
}

void qualified_id::push(eve::ast::simple_id* id)
{
	this->_ids.push_back(id);
}

const eve::ast::simple_id* qualified_id::simple() const
{
	return this->_ids.back();
}

void qualified_id::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

const std::vector<eve::ast::simple_id*> qualified_id::ids() const
{
	return this->_ids;
}

std::string qualified_id::string() const
{
	std::string string = this->_ids[0]->string();

	for (size_t i = 1; i < this->_ids.size(); i++)
		string += "::" + this->_ids[i]->string(); 

	return string;
}

eve::ast::qualified_id* qualified_id::dup() const
{
	std::vector<eve::ast::simple_id*> ids;

	for (size_t i = 1; i < this->_ids.size(); i++)
		ids.push_back(this->_ids[i]->dup());

	eve::ast::qualified_id* qualified = new eve::ast::qualified_id(this->location(), ids);
	qualified->_parent = this->_parent;

	return qualified;
}

}
}

#endif