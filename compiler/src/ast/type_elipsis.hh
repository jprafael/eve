#ifndef EVE_AST_TYPE_ELIPSIS_IMPL
#define EVE_AST_TYPE_ELIPSIS_IMPL

#include "../visitor/visitor.hh"

namespace eve
{
namespace ast
{

type_elipsis::type_elipsis(const eve::location& location, eve::ast::type* base)
	: eve::ast::type(location), _base(base)
{
	this->adopt(base);
}

type_elipsis::~type_elipsis()
{
	delete this->_base;
}

void type_elipsis::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

std::string type_elipsis::translate(eve::driver& driver, eve::target::cu& cu)
{
	driver.debug(this->location(), "translate type_elipsis");
	return this->_base->translate(driver, cu) + "...";
}

eve::ast::type* type_elipsis::dup() const
{
	eve::ast::type_elipsis* elipsis = new eve::ast::type_elipsis(this->location(), this->_base->dup());
	elipsis->_parent = this->_parent;
	return elipsis;
}

eve::ast::type_base* type_elipsis::base() const
{
	return this->_base->base();
}

eve::ast::type* type_elipsis::bind(std::map<std::string, eve::ast::argument_type*> bindings) const
{
	assert(false);
	eve::ast::type_elipsis* t = new eve::ast::type_elipsis(
		this->location(),
		this->_base->bind(bindings)
	);
	t->_parent = this->_parent;

	return t;
}

bool type_elipsis::is(const std::string& id) const
{
	return id == "elipsis";
}

bool type_elipsis::equals(const eve::ast::type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const
{
	if (this == rhs)
		return true;

	const eve::ast::type_elipsis* base = dynamic_cast<const eve::ast::type_elipsis*>(rhs);
	if (!base)
		return false;

	return this->_base->equals(base->_base, out_bindings);
}

bool type_elipsis::assignable(eve::driver& driver, const eve::ast::type* rhs, std::map<std::string, eve::ast::argument_type*>& out_bindings) const
{
	if (this->equals(rhs, out_bindings))
		return true;

	driver.debug("type_elipsis::assignable()");
	return false;
}

std::map<std::string, eve::ast::argument_type*> type_elipsis::bindings() const
{
	return this->base()->bindings();
}

bool type_elipsis::requires_conversion(const eve::ast::type* rhs) const
{
	assert(false);
	return true;
}


}
}

#endif