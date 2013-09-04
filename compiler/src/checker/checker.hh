#ifndef EVE_CHECKER_HH
#define EVE_CHECKER_HH

#include "../ast/ast.hh"

namespace eve
{

namespace checker
{

class checker
{
	eve::driver& driver;

public:
	checker(eve::driver& driver);
	virtual ~checker();

	eve::ast::type* resolve
	(
		std::string name,
		const eve::symbol::context& context,
		std::map<std::string, eve::ast::argument_type*> bindings,
		std::vector<const eve::ast::type*> arguments,
		std::vector<eve::ast::type*>& out_arguments,
		bool is_call,
		bool allow_conversions = true
	);

	eve::ast::definition_type* get_builtin(std::string name);

	eve::ast::type* make_bool();
	eve::ast::type* make_char(size_t bits);
	eve::ast::type* make_int(size_t bits);
	eve::ast::type* make_float(size_t bits);
	eve::ast::type* make_string();

	eve::ast::type* make_shared(eve::ast::type* base);
	eve::ast::type* make_static(eve::ast::type* base);

	eve::ast::type* make_type(eve::ast::simple_id* id);
	eve::ast::type* make_function(eve::ast::type* ret, std::vector<eve::ast::type*> parameters);

};

}

}

#endif