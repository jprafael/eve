#ifndef EVE_AST_OP_IMPL
#define EVE_AST_OP_IMPL

namespace eve
{
namespace ast
{

std::string to_string(const eve::ast::op& op)
{
	switch (op)
	{
		case assign_eq:
			return "=";

		case assign_shift_r:
			return ">>=";

		case assign_shift_l:
			return "<<=";

		case assign_add:
			return "+=";

		case assign_sub:
			return "-=";

		case assign_mul:
			return "*=";

		case assign_div:
			return "/=";

		case assign_mod:
			return "%%=";

		case assign_and:
			return "&=";

		case assign_or:
			return "|=";

		case assign_bool_and:
			return "&&=";

		case assign_bool_or:
			return "||=";

		case assign_xor:
			return "^=";

		case assign_power:
			return "TODO: ASSIGN_POWER";

		case incr_inc:
			return "++";

		case incr_dec:
			return "--";

		case prefix_plus:
			return "+";

		case prefix_minus:
			return "-";

		case prefix_not:
			return "~";

		case prefix_bool_not:
			return "!";

		case infix_add:
			return "+";

		case infix_sub:
			return "-";

		case infix_div:
			return "/";

		case infix_xor:
			return "^";

		case infix_and:
			return "&";

		case infix_or:
			return "|";

		case infix_mod:
			return "%%";

		case infix_shift_r:
			return ">>";

		case infix_shift_l:
			return "<<";

		case infix_bool_and:
			return "&&";

		case infix_bool_or:
			return "||";

		case infix_power:
			return "INFIX_POWER";

		case infix_l:
			return "<";

		case infix_g:
			return ">";

		case infix_le:
			return "<=";

		case infix_ge:
			return ">=";

		case infix_ne:
			return "!=";

		case infix_ne3:
			return "NE3";

		case infix_eq:
			return "==";

		case infix_eq3:
			return "EQ3";

		case n_ary_parens:
			return "()";

		case n_ary_bracks:
			return "[]";

		case asterisc:
			return "*";
	}

	return "INVALID_OP";
}

}
}

#endif
