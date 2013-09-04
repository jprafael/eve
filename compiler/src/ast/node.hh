#ifndef EVE_AST_NODE_IMPL
#define EVE_AST_NODE_IMPL

#include <cassert>

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

node::node(const eve::location& location)
	: _parent(NULL), _location(location)
{
}

node::~node()
{
}

void node::adopt(eve::ast::node* child)
{
	/* ignore NULL subnodes */
	if (!child)
		return;

	this->_children.push_back(child);
	child->_parent = this;
}

template <class T>
void node::adopt_all(const std::vector<T*>& children)
{
	for (size_t i = 0; i < children.size(); i++)
		this->adopt(children[i]);
}

/* force this one to be generated */
template void node::adopt_all<eve::ast::node>(const std::vector<eve::ast::node*>& children);

eve::ast::node*& node::parent()
{
	return this->_parent;
}

std::vector<eve::ast::node*>& node::children()
{
	return this->_children;
}

eve::symbol::context& node::context()
{
	return this->_parent->context();
}

const eve::symbol::context& node::context() const
{
	return this->_parent->context();
}

const eve::location& node::location() const
{
	return this->_location;
}

void node::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

}
}

#endif

