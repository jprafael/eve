#ifndef EVE_AST_EXPR_IMPL
#define EVE_AST_EXPR_IMPL

namespace eve
{
namespace ast
{

expr::expr(const eve::location& location)
	: eve::ast::node(location), _type(NULL), _expected(NULL)
{
}

expr::~expr()
{
	if (this->_type)
		delete this->_type;
}

void expr::accept(eve::visitor::visitor& visitor)
{
	visitor.visit(this);
}

eve::ast::type* expr::type()
{
	return this->_type;
}

std::string expr::wrap(std::string value, eve::driver& driver, eve::target::cu& cu)
{
	bool up_cast; /* make a shared object */
	bool down_cast; /* get the object out of the shared*/

	assert (this->_type);

	if (this->_type->is("shared"))
	{
		up_cast = false;

		if (this->_expected && this->_expected->is("shared"))
			down_cast = false;

		else
			down_cast = true;

	} else
	{
		down_cast = false;

		if (this->_type)
			std::cerr << "type:" << this->_type->translate(driver, cu) << std::endl;

		else
			std::cerr << "notype\n";
		
		if (this->_expected)
			std::cerr << "expc:" << this->_expected->translate(driver, cu) << std::endl;
		else
			std::cerr << "noexp\n";

		if (this->_expected && this->_expected->is("shared"))
			up_cast = true;

		else
			up_cast = false;
	}		

	std::string ret;

	if (up_cast)
 	{
 		eve::ast::argument_type* base = this->_expected->bindings()["T"];
 		assert(base);

 		/* automatic up casting to shared */
 		ret += "eve::rt::make_shared<" + base->translate(driver, cu) + ">(";

 	} else if (down_cast)
 	{
 		/* automatic down casting from shared */
		ret += "(*";
	}

	ret += value;

	if (up_cast || down_cast)
		ret += ")";

	return ret;	
}

}
}

#endif
