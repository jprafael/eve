%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

#include "ast/ast.hh"

%}

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

/* start symbol is named "start" */
%start start

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="eve"

/* set the parser's class identifier */
%define "parser_class_name" "parser"

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.initialize(const_cast<std::string*>(driver.filename()));
 };

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class driver& driver }

/* verbose error messages */
%error-verbose

 /*** BEGIN EVE - Change the eve grammar's tokens below ***/

%union {
	eve::ast::id*					id;
	eve::ast::simple_id*			simple_id;
	eve::ast::qualified_id* 		qualified_id;

	eve::ast::constant*				constant;

	eve::ast::module*				module;
	eve::ast::import*				import;

	eve::ast::definition*			definition;
	eve::ast::definition_function*  definition_function;
	eve::ast::definition_variable*	definition_variable;
	eve::ast::definition_type*		definition_type;

	eve::ast::type*					type;

	eve::ast::argument_type*				argument_type;
	eve::ast::argument_function_named*		argument_function_named;

	eve::ast::parameter_type*		parameter_type;
	eve::ast::parameter_function* 	parameter_function;

	eve::ast::expr*					expr;
	eve::ast::expr_unary*			expr_unary;
	eve::ast::expr_binary*			expr_binary;
	eve::ast::expr_incr*			expr_incr;
	eve::ast::expr_prefix*			expr_prefix;
	eve::ast::expr_infix*			expr_infix;
	eve::ast::expr_assign*			expr_assign;
	eve::ast::expr_n_ary*			expr_n_ary;
	eve::ast::expr_lambda*			expr_lambda;

	eve::ast::stmt*					stmt;
	eve::ast::stmt_expr*			stmt_expr;
	eve::ast::stmt_if*				stmt_if;
	eve::ast::stmt_while*			stmt_while;
	eve::ast::stmt_for* 			stmt_for;
	eve::ast::stmt_for_in* 			stmt_for_in;
	eve::ast::stmt_return* 			stmt_return;
	eve::ast::stmt_par* 			stmt_par;
	eve::ast::stmt_on* 				stmt_on;
	eve::ast::stmt_finish* 			stmt_finish;

	eve::ast::perm*					perm;

	eve::ast::op					op;

	/* list s*/
	std::vector<eve::ast::import*>*					import_list;
	std::vector<eve::ast::definition*>*				definition_list;

	std::vector<eve::ast::argument_function_named*>*	argument_function_named_list;

	std::vector<eve::ast::stmt*>*					stmt_list;
	std::vector<eve::ast::expr*>*					expr_list;
	std::vector<eve::ast::perm*>*					perm_list;

	std::vector<eve::ast::type*>*					type_list;
	std::vector<eve::ast::argument_type*>*			argument_type_list;

	std::vector<eve::ast::parameter_type*>*			parameter_type_list;
	std::vector<eve::ast::parameter_function*>*		parameter_function_list;
}

%type <id> id
%type <qualified_id> qualified_id

%type <module> module
%type <import> import

%type <type> type type_no_id;

%type <parameter_type> parameter_type
%type <parameter_function> parameter_function

%type <definition> definition
%type <definition_function> definition_function
%type <definition_variable> definition_variable
%type <definition_type> definition_type

%type <argument_type> argument_type;
%type <argument_function_named> argument_function_named

%type <stmt> stmt
%type <stmt_expr> stmt_expr
%type <stmt_if> stmt_if
%type <stmt_while> stmt_while
%type <stmt_for> stmt_for
%type <stmt_for_in> stmt_for_in
%type <stmt_return> stmt_return
%type <stmt_par> stmt_par
%type <stmt_on> stmt_on
%type <stmt_finish> stmt_finish

%type <expr> for_init for_cond for_incr

%type <expr> expr_inline expr_block
%type <expr_unary> expr_inline_unary expr_block_unary
%type <expr_binary> expr_inline_binary expr_block_binary
%type <expr_incr> expr_inline_incr expr_block_incr
%type <expr_prefix> expr_inline_prefix expr_block_prefix
%type <expr_infix> expr_inline_infix expr_block_infix
%type <expr_assign> expr_inline_assign expr_block_assign
%type <expr_n_ary> expr_inline_n_ary expr_block_n_ary
%type <expr_lambda> expr_inline_lambda expr_block_lambda

%type <perm> perm

%type <op> operator

%type <import_list> import_list
%type <definition_list> definition_list
%type <argument_function_named_list> argument_function_named_list
%type <stmt_list> stmt_list stmt_block stmt_or_block
%type <expr_list> expr_inline_list optional_expr_inline_list
%type <parameter_function_list> parameter_function_list optional_parameter_function_list
%type <argument_type_list> argument_type_list
%type <parameter_type_list> parameter_type_list optional_parameter_type_list
%type <perm_list> optional_perms perms perm_list
%type <type_list> type_list optional_type_list

%token					END			0	"end of file"
%token					EOL				"end of line"
%token					INDENT			"indent"
%token					DEDENT			"dedent"

%token					IMPORT			"import"
%token 					AS				"as"
%token 					BUILTIN			"builtin"

%token					TYPE			"class"
%token					VAR				"var"
%token					SELF			"self"
%token					OPERATOR		"operator"

%token					PASS			"pass"
%token					IN				"in"

%token					ELIPSIS			"elipsis"

%token					IF				"if"
%token					ELSE			"else"
%token					FOR				"for"
%token					WHILE			"while"
%token					RETURN			"return"
%token					ON				"on"
%token					FINISH			"finish"
%token					CONSTRUCTOR		"constructor"

/*
%token					STATIC			"static"
%token					FINAL			"final"
%token					NEW				"new"
%token					PRIVATE			"private"
%token					PUBLIC			"public"
%token					SWITCH			"switch"
%token					BREAK			"break"
%token					CASE			"case"
%token					CONTINUE		"continue"
%token					DEFAULT			"default"
%token					DO				"do"
*/

%token					ASSIGN_SHIFT_R
%token					ASSIGN_SHIFT_L
%token					ASSIGN_ADD
%token					ASSIGN_SUB
%token					ASSIGN_MUL
%token					ASSIGN_DIV
%token					ASSIGN_MOD
%token					ASSIGN_AND
%token					ASSIGN_OR
%token					ASSIGN_BOOL_AND
%token					ASSIGN_BOOL_OR
%token					ASSIGN_XOR
%token					ASSIGN_POWER

%token					OP_SHIFT_R
%token					OP_SHIFT_L

%token					OP_INC
%token					OP_DEC
%token					OP_BOOL_AND
%token					OP_BOOL_OR
%token					OP_LE
%token					OP_GE
%token					OP_EQ3
%token					OP_NE3
%token					OP_EQ
%token					OP_NE
%token					OP_POWER

%token <constant> 		CONSTANT		"constant"
%token <simple_id>		SIMPLE_ID		"simple_id"

%destructor { delete $$; } <*>
%destructor { } <op>

/* PRIORITIES */
%nonassoc LOW_PREC

%right '=' ASSIGN_SHIFT_R ASSIGN_SHIFT_L ASSIGN_ADD ASSIGN_SUB ASSIGN_MUL ASSIGN_DIV ASSIGN_MOD ASSIGN_AND ASSIGN_XOR ASSIGN_OR ASSIGN_BOOL_AND ASSIGN_BOOL_OR ASSIGN_POWER
%left '?' IF
%left ':' ELSE
%left OP_BOOL_OR
%left OP_BOOL_AND
%left '|'
%left '^'
%left '&'
%left OP_EQ3 OP_NE3
%left OP_EQ OP_NE
%left '>' OP_GE
%left '<' OP_LE
%left OP_SHIFT_R OP_SHIFT_L
%left '+' '-'
%left '*' '%' '/'
%left OP_POWER
%right OP_INC OP_DEC '~' '!'
%left '.'
%left '(' ')' '[' ']'
%right PREFIX EXPR_LAMBDA 

%left REDUCE

 /*** END EVE - Change the eve grammar's tokens above ***/

%{

#include "driver.hh"
#include "scanner.hh"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}

%code requires {
#include "ast/ast.hh"
}

%% /*** Grammar Rules ***/

 /*** BEGIN EVE - Change the eve grammar rules below ***/

qualified_id
	: SIMPLE_ID '.' SIMPLE_ID				{ $$ = new eve::ast::qualified_id(yylloc, $1, $3); }
	| qualified_id '.' SIMPLE_ID			{ $$ = $1; $$->push($3); }
	;

id
	: SIMPLE_ID								{ $$ = $1; }
	| qualified_id							{ $$ = $1; }
	;

import
	: IMPORT id								{ $$ = new eve::ast::import_simple(yylloc, $2); }
	| IMPORT id AS SIMPLE_ID				{ $$ = new eve::ast::import_simple(yylloc, $2, $4); }
	| IMPORT id ':' ':' '*'					{ $$ = new eve::ast::import_all(yylloc, $2); }
	;

import_list
	: import EOL							{ $$ = new std::vector<eve::ast::import*>(); $$->push_back($1); }
	| import_list import EOL				{ $$ = $1; $1->push_back($2); }
	;

operator
	: '!'				{ $$ = eve::ast::op::prefix_not; }
	| '~'				{ $$ = eve::ast::op::prefix_bool_not; }

	| '='				{ $$ = eve::ast::op::assign_eq; }
	| ASSIGN_SHIFT_R	{ $$ = eve::ast::op::assign_shift_r; }
	| ASSIGN_SHIFT_L	{ $$ = eve::ast::op::assign_shift_l; }
	| ASSIGN_ADD		{ $$ = eve::ast::op::assign_add; }
	| ASSIGN_SUB		{ $$ = eve::ast::op::assign_sub; }
	| ASSIGN_MUL		{ $$ = eve::ast::op::assign_mul; }
	| ASSIGN_DIV		{ $$ = eve::ast::op::assign_div; }
	| ASSIGN_MOD		{ $$ = eve::ast::op::assign_mod; }
	| ASSIGN_AND		{ $$ = eve::ast::op::assign_and; }
	| ASSIGN_OR			{ $$ = eve::ast::op::assign_or; }
	| ASSIGN_BOOL_AND	{ $$ = eve::ast::op::assign_bool_and; }
	| ASSIGN_BOOL_OR	{ $$ = eve::ast::op::assign_bool_or; }
	| ASSIGN_XOR		{ $$ = eve::ast::op::assign_xor; }
	| ASSIGN_POWER		{ $$ = eve::ast::op::assign_power; }

	| OP_INC			{ $$ = eve::ast::op::incr_inc; }
	| OP_DEC			{ $$ = eve::ast::op::incr_dec; }

	| '+'				{ $$ = eve::ast::op::infix_add; }
	| '-'				{ $$ = eve::ast::op::infix_sub; }
	| '/'				{ $$ = eve::ast::op::infix_div; }
	| '^'				{ $$ = eve::ast::op::infix_xor; }
	| '&'				{ $$ = eve::ast::op::infix_and; }
	| '|'				{ $$ = eve::ast::op::infix_or; }
	| '%'				{ $$ = eve::ast::op::infix_mod; }
	| '<'				{ $$ = eve::ast::op::infix_l; }
	| '>'				{ $$ = eve::ast::op::infix_g; }
	| OP_SHIFT_R		{ $$ = eve::ast::op::infix_shift_r; }
	| OP_SHIFT_L		{ $$ = eve::ast::op::infix_shift_l; }
	| OP_BOOL_AND		{ $$ = eve::ast::op::infix_bool_and; }
	| OP_BOOL_OR		{ $$ = eve::ast::op::infix_bool_or; }
	| OP_LE				{ $$ = eve::ast::op::infix_le; }
	| OP_GE				{ $$ = eve::ast::op::infix_ge; }
	| OP_EQ				{ $$ = eve::ast::op::infix_eq; }
	| OP_EQ3			{ $$ = eve::ast::op::infix_eq3; }
	| OP_NE				{ $$ = eve::ast::op::infix_ne; }
	| OP_NE3			{ $$ = eve::ast::op::infix_ne3; }
	| OP_POWER			{ $$ = eve::ast::op::infix_power; }

	| '(' ')'			{ $$ = eve::ast::op::n_ary_parens; }
	| '[' ']'			{ $$ = eve::ast::op::n_ary_bracks; }

	| '*'				{ $$ = eve::ast::op::asterisc; }
	;

definition_function
	: CONSTRUCTOR ':' optional_parameter_type_list optional_perms '(' optional_parameter_function_list ')' ':' stmt_or_block			{ $$ = new eve::ast::definition_function_named_explicit(yylloc, NULL, $3, $6, $4, NULL, $9); }
	| SIMPLE_ID ':' optional_parameter_type_list optional_perms '(' optional_parameter_function_list ')' type ':' stmt_or_block			{ $$ = new eve::ast::definition_function_named_explicit(yylloc, $8, $3, $6, $4, $1, $10); }
	| operator ':' optional_parameter_type_list optional_perms '(' optional_parameter_function_list ')' type ':' stmt_or_block			{ $$ = new eve::ast::definition_function_operator_explicit(yylloc, $8, $3, $6, $4, $1, $10); }
	;

definition_variable
	: SIMPLE_ID ':' type EOL																					{ $$ = new eve::ast::definition_variable(yylloc, $3, $1, new std::vector<eve::ast::expr*>(), new std::vector<eve::ast::argument_function_named*>()); }
	| SIMPLE_ID ':' type '(' optional_expr_inline_list ')' EOL													{ $$ = new eve::ast::definition_variable(yylloc, $3, $1, $5, new std::vector<eve::ast::argument_function_named*>()); }
	| SIMPLE_ID ':' type '(' optional_expr_inline_list ')' ':' EOL INDENT argument_function_named_list DEDENT 	{ $$ = new eve::ast::definition_variable(yylloc, $3, $1, $5, $10); }
	| SIMPLE_ID ':' type '=' expr_inline EOL																	{ $$ = new eve::ast::definition_variable(yylloc, $3, $1, $5); }
	| SIMPLE_ID ':' type '=' expr_block																			{ $$ = new eve::ast::definition_variable(yylloc, $3, $1, $5); }
	;

definition_type
	: TYPE SIMPLE_ID optional_parameter_type_list optional_type_list ':' EOL INDENT definition_list DEDENT { $$ = new eve::ast::definition_type(yylloc, $2, $3, $4, $8); }
	| TYPE SIMPLE_ID optional_parameter_type_list optional_type_list ':' EOL INDENT PASS EOL DEDENT { $$ = new eve::ast::definition_type(yylloc, $2, $3, $4, new std::vector<eve::ast::definition*>()); }
	;

optional_type_list
	: /* empty */							{ $$ = new std::vector<eve::ast::type*>(); }
	| '(' ')'								{ $$ = new std::vector<eve::ast::type*>(); }
	| '(' type_list ')'						{ $$ = $2; }
	;

type_list
	: type									{ $$ = new std::vector<eve::ast::type*>(); $$->push_back($1); }
	| type_list ',' type					{ $$ = $1; $1->push_back($3); }
	;

optional_parameter_type_list
	: /* empty */							{ $$ = new std::vector<eve::ast::parameter_type*>(); }
	| '<' parameter_type_list '>'			{ $$ = $2; }
	;

parameter_type_list
	: parameter_type							{ $$ = new std::vector<eve::ast::parameter_type*>(); $$->push_back($1); }
	| parameter_type_list ',' parameter_type	{ $$ = $1; $1->push_back($3); }
	;

parameter_type
	: TYPE SIMPLE_ID						{ $$ = new eve::ast::parameter_type_type(yylloc, $2, NULL); }
	| TYPE SIMPLE_ID '=' type				{ $$ = new eve::ast::parameter_type_type(yylloc, $2, $4); }
	| TYPE SIMPLE_ID ELIPSIS				{ $$ = new eve::ast::parameter_type_type(yylloc, $2); }
	| VAR SIMPLE_ID							{ $$ = new eve::ast::parameter_type_constant(yylloc, $2, NULL); }
	| VAR SIMPLE_ID '=' CONSTANT			{ $$ = new eve::ast::parameter_type_constant(yylloc, $2, $4); }
	| VAR SIMPLE_ID ELIPSIS					{ $$ = new eve::ast::parameter_type_constant(yylloc, $2); }
	| argument_type							{ $$ = new eve::ast::parameter_type_specialization(yylloc, $1); }
 	;

definition
	: definition_function					{ $$ = $1; }
	| BUILTIN definition_function			{ $$ = $2; $2->set_builtin(true); }
	| definition_variable					{ $$ = $1; }
	| BUILTIN definition_variable			{ $$ = $2; $2->set_builtin(true); }
	| definition_type						{ $$ = $1; }
	| BUILTIN definition_type				{ $$ = $2; $2->set_builtin(true); }
	;

definition_list 
	: definition							{ $$ = new std::vector<eve::ast::definition*>(); $$->push_back($1); }
	| definition_list definition 			{ $$ = $1; $1->push_back($2); }
	;

expr_inline
	: CONSTANT																					{ $$ = new eve::ast::expr_constant(yylloc, $1); }
	| SELF																						{ $$ = new eve::ast::expr_self(yylloc); }
	| SIMPLE_ID																					{ $$ = new eve::ast::expr_id(yylloc, $1); }
 	| expr_inline '.' SIMPLE_ID																	{ $$ = new eve::ast::expr_field(yyloc, $1, $3); }
	| expr_inline_unary																			{ $$ = $1; }
	| '(' expr_inline ')'																		{ $$ = $2; }
	| expr_inline_binary																		{ $$ = $1; }
	| expr_inline_n_ary																			{ $$ = $1; }
	| expr_inline_lambda																		{ $$ = $1; }
	;

expr_block
	: expr_block_unary						{ $$ = $1; }
	| expr_block_binary						{ $$ = $1; }
	| expr_block_n_ary						{ $$ = $1; }
	| expr_block_lambda						{ $$ = $1; }
	;

expr_inline_unary
	: expr_inline_incr						{ $$ = $1; }
	| expr_inline_prefix					{ $$ = $1; }
	;

expr_block_unary
	: expr_block_incr						{ $$ = $1; }
	| expr_block_prefix						{ $$ = $1; }
	;

expr_inline_incr
	: OP_INC expr_inline					{ $$ = new eve::ast::expr_incr(yylloc, $2, eve::ast::op::incr_inc, true); }
	| expr_inline OP_INC 					{ $$ = new eve::ast::expr_incr(yylloc, $1, eve::ast::op::incr_inc, false); }
	| OP_DEC expr_inline					{ $$ = new eve::ast::expr_incr(yylloc, $2, eve::ast::op::incr_dec, true); }
	| expr_inline OP_DEC					{ $$ = new eve::ast::expr_incr(yylloc, $1, eve::ast::op::incr_dec, false); }
	;

expr_block_incr
	: OP_INC expr_block						{ $$ = new eve::ast::expr_incr(yylloc, $2, eve::ast::op::incr_inc, true); }
	| OP_DEC expr_block						{ $$ = new eve::ast::expr_incr(yylloc, $2, eve::ast::op::incr_dec, true); }
	;

expr_inline_prefix
	: '+' expr_inline %prec PREFIX			{ $$ = new eve::ast::expr_prefix(yylloc, $2, eve::ast::op::prefix_plus); }
	| '-' expr_inline %prec PREFIX			{ $$ = new eve::ast::expr_prefix(yylloc, $2, eve::ast::op::prefix_minus); }
	| '!' expr_inline %prec PREFIX			{ $$ = new eve::ast::expr_prefix(yylloc, $2, eve::ast::op::prefix_bool_not); }
	| '~' expr_inline %prec PREFIX			{ $$ = new eve::ast::expr_prefix(yylloc, $2, eve::ast::op::prefix_not); }
	| '*' expr_inline %prec PREFIX			{ $$ = new eve::ast::expr_prefix(yylloc, $2, eve::ast::op::asterisc); }
	;

expr_block_prefix
	: '+' expr_block %prec PREFIX			{ $$ = new eve::ast::expr_prefix(yylloc, $2, eve::ast::op::prefix_plus); }
	| '-' expr_block %prec PREFIX			{ $$ = new eve::ast::expr_prefix(yylloc, $2, eve::ast::op::prefix_minus); }
	| '!' expr_block %prec PREFIX			{ $$ = new eve::ast::expr_prefix(yylloc, $2, eve::ast::op::prefix_bool_not); }
	| '~' expr_block %prec PREFIX			{ $$ = new eve::ast::expr_prefix(yylloc, $2, eve::ast::op::prefix_not); }
	| '*' expr_block %prec PREFIX			{ $$ = new eve::ast::expr_prefix(yylloc, $2, eve::ast::op::asterisc); }
	;

expr_inline_binary
	: expr_inline_infix						{ $$ = $1; }
	| expr_inline_assign					{ $$ = $1; }
	;

expr_block_binary
	: expr_block_infix						{ $$ = $1; }
	| expr_block_assign						{ $$ = $1; }
	;

expr_inline_assign
	: expr_inline '=' expr_inline							{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_eq); } /* TODO */
	| expr_inline ASSIGN_SHIFT_R expr_inline				{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_shift_r); }
	| expr_inline ASSIGN_SHIFT_L expr_inline				{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_shift_l); }
	| expr_inline ASSIGN_ADD expr_inline	 				{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_add); }
	| expr_inline ASSIGN_SUB expr_inline					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_sub); }
	| expr_inline ASSIGN_MUL expr_inline					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_mul); }
	| expr_inline ASSIGN_DIV expr_inline					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_div); }
	| expr_inline ASSIGN_MOD expr_inline					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_mod); }
	| expr_inline ASSIGN_AND expr_inline					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_and); }
	| expr_inline ASSIGN_OR expr_inline						{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_or); }
	| expr_inline ASSIGN_BOOL_AND expr_inline				{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_bool_and); }
	| expr_inline ASSIGN_BOOL_OR expr_inline				{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_bool_or); }
	| expr_inline ASSIGN_XOR expr_inline					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_xor); }
	| expr_inline ASSIGN_POWER expr_inline					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_power); }
	;

expr_block_assign
	: expr_inline '=' expr_block							{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_eq); } /* TODO */
	| expr_inline ASSIGN_SHIFT_R expr_block					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_shift_r); }
	| expr_inline ASSIGN_SHIFT_L expr_block					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_shift_l); }
	| expr_inline ASSIGN_ADD expr_block	 					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_add); }
	| expr_inline ASSIGN_SUB expr_block						{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_sub); }
	| expr_inline ASSIGN_MUL expr_block						{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_mul); }
	| expr_inline ASSIGN_DIV expr_block						{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_div); }
	| expr_inline ASSIGN_MOD expr_block						{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_mod); }
	| expr_inline ASSIGN_AND expr_block						{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_and); }
	| expr_inline ASSIGN_OR expr_block						{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_or); }
	| expr_inline ASSIGN_BOOL_AND expr_block				{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_bool_and); }
	| expr_inline ASSIGN_BOOL_OR expr_block					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_bool_or); }
	| expr_inline ASSIGN_XOR expr_block						{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_xor); }
	| expr_inline ASSIGN_POWER expr_block					{ $$ = new eve::ast::expr_assign(yylloc, $1, $3, eve::ast::op::assign_power); }
	;

expr_inline_infix
	: expr_inline '+' expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_add); }
	| expr_inline '-' expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_sub); }
	| expr_inline '*' expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::asterisc); }
	| expr_inline '/' expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_div); }
	| expr_inline '^' expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_xor); }
	| expr_inline '&' expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_and); }
	| expr_inline '|' expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_or); }
	| expr_inline '%' expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_mod); }
	| expr_inline '<' expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_l); }
	| expr_inline '>' expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_g); }
	| expr_inline '<' '<' expr_inline						{ $$ = new eve::ast::expr_infix(yylloc, $1, $4, eve::ast::op::infix_shift_r); }
	| expr_inline '>' '>' expr_inline						{ $$ = new eve::ast::expr_infix(yylloc, $1, $4, eve::ast::op::infix_shift_l); }
	| expr_inline OP_BOOL_AND expr_inline					{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_bool_and); }
	| expr_inline OP_BOOL_OR expr_inline					{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_bool_or); }
	| expr_inline OP_LE expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_le); }
	| expr_inline OP_GE expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_ge); }
	| expr_inline OP_EQ expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_eq); }
	| expr_inline OP_EQ3 expr_inline						{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_eq3); }
	| expr_inline OP_NE expr_inline							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_ne); }
	| expr_inline OP_NE3 expr_inline						{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_ne3); }
	| expr_inline OP_POWER expr_inline						{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_power); }
	;

expr_block_infix
	: expr_inline '+' expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_add); }
	| expr_inline '-' expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_sub); }
	| expr_inline '*' expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::asterisc); }
	| expr_inline '/' expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_div); }
	| expr_inline '^' expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_xor); }
	| expr_inline '&' expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_and); }
	| expr_inline '|' expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_or); }
	| expr_inline '%' expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_mod); }
	| expr_inline '<' expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_l); }
	| expr_inline '>' expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_g); }
	| expr_inline '<' '<' expr_block						{ $$ = new eve::ast::expr_infix(yylloc, $1, $4, eve::ast::op::infix_shift_r); }
	| expr_inline '>' '>' expr_block						{ $$ = new eve::ast::expr_infix(yylloc, $1, $4, eve::ast::op::infix_shift_l); }
	| expr_inline OP_BOOL_AND expr_block					{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_bool_and); }
	| expr_inline OP_BOOL_OR expr_block						{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_bool_or); }
	| expr_inline OP_LE expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_le); }
	| expr_inline OP_GE expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_ge); }
	| expr_inline OP_EQ expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_eq); }
	| expr_inline OP_EQ3 expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_eq3); }
	| expr_inline OP_NE expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_ne); }
	| expr_inline OP_NE3 expr_block							{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_ne3); }
	| expr_inline OP_POWER expr_block						{ $$ = new eve::ast::expr_infix(yylloc, $1, $3, eve::ast::op::infix_power); }
	;

/* TODO argument_function_named_list */
expr_inline_n_ary
	: expr_inline '[' optional_expr_inline_list ']' 	{ $$ = new eve::ast::expr_n_ary(yylloc, $1, eve::ast::op::n_ary_bracks, $3, new std::vector<eve::ast::argument_function_named*>()); }
	| expr_inline '(' optional_expr_inline_list ')'		{ $$ = new eve::ast::expr_n_ary(yylloc, $1, eve::ast::op::n_ary_parens, $3, new std::vector<eve::ast::argument_function_named*>()); }
	;

expr_block_n_ary									
	: expr_inline '[' optional_expr_inline_list ']'	':' EOL INDENT argument_function_named_list DEDENT { $$ = new eve::ast::expr_n_ary(yylloc, $1, eve::ast::op::n_ary_bracks, $3, $8); }
	| expr_inline '(' optional_expr_inline_list ')'	':' EOL INDENT argument_function_named_list DEDENT { $$ = new eve::ast::expr_n_ary(yylloc, $1, eve::ast::op::n_ary_parens, $3, $8); }
	;

expr_inline_lambda
	: perms '(' optional_parameter_function_list ')' type ':' expr_inline		{ $$ = new eve::ast::expr_lambda(yylloc, $5, $3, $1, $7); }
	;

expr_block_lambda
	: perms '(' optional_parameter_function_list ')' type ':' EOL stmt_block 	{ $$ = new eve::ast::expr_lambda(yylloc, $5, $3, $1, $8); }
	;

argument_function_named
	: SIMPLE_ID '=' expr_inline EOL						{ $$ = new eve::ast::argument_function_named(yylloc, $1, $3); }
	| SIMPLE_ID '=' expr_block							{ $$ = new eve::ast::argument_function_named(yylloc, $1, $3); }
	;

argument_function_named_list
	: argument_function_named								{ $$ = new std::vector<eve::ast::argument_function_named*>(); $$->push_back($1); }
	| argument_function_named_list argument_function_named	{ $$ = $1; $1->push_back($2); }
	;

expr_inline_list 
	: expr_inline							{ $$ = new std::vector<eve::ast::expr*>(); $$->push_back($1); }
	| expr_inline_list ',' expr_inline		{ $$ = $1; $1->push_back($3); }
	;

optional_expr_inline_list
	: /* EMPTY */							{ $$ = new std::vector<eve::ast::expr*>(); }
	| expr_inline_list						{ $$ = $1; }
	;

stmt
	: PASS EOL								{ $$ = new eve::ast::stmt_pass(yylloc); }
	| definition							{ $$ = new eve::ast::stmt_definition(yylloc, $1); }
	| stmt_expr								{ $$ = $1; }
	| stmt_if								{ $$ = $1; }
	| stmt_while							{ $$ = $1; }
	| stmt_for								{ $$ = $1; }
	| stmt_for_in							{ $$ = $1; }
	| stmt_return							{ $$ = $1; }
	| stmt_par								{ $$ = $1; }
	| stmt_on								{ $$ = $1; }
	| stmt_finish							{ $$ = $1; }
	;

stmt_if
	: IF expr_inline ':' stmt_or_block								{ $$ = new eve::ast::stmt_if(yylloc, $2, $4, NULL); }
	| IF expr_inline ':' EOL stmt_block ELSE ':' EOL stmt_block		{ $$ = new eve::ast::stmt_if(yylloc, $2, $5, $9); }
	;

stmt_while
	: WHILE expr_inline ':' stmt_or_block							{ $$ = new eve::ast::stmt_while(yylloc, $2, $4); }
	;

stmt_for
	: FOR '(' for_init ';' for_cond ';' for_incr ')' ':' stmt_or_block						{ $$ = new eve::ast::stmt_for(yylloc, $3, $5, $7, $10); }
	| '@' FOR '(' for_init ';' for_cond ';' for_incr ')' optional_perms ':' stmt_or_block	{ $$ = new eve::ast::stmt_for(yylloc, $4, $6, $8, $12, $10); }
	;

stmt_for_in
	: FOR '(' SIMPLE_ID IN expr_inline ')' ':' stmt_or_block								{ $$ = new eve::ast::stmt_for_in(yylloc, $3, $5, $8); }
	| '@' FOR '(' SIMPLE_ID IN expr_inline ')' optional_perms ':' stmt_or_block				{ $$ = new eve::ast::stmt_for_in(yylloc, $4, $6, $10, $8); }
	;

for_init
	: /* EMPTY */										{ $$ = NULL; }
	| expr_inline										{ $$ = $1; }
	;

for_cond
	: /* EMPTY */										{ $$ = NULL; }
	| expr_inline										{ $$ = $1; }
	;

for_incr
	: /* EMPTY */										{ $$ = NULL; }
	| expr_inline										{ $$ = $1; }
	;

stmt_expr
	: expr_inline EOL									{ $$ = new eve::ast::stmt_expr(yylloc, $1); }
	| expr_block										{ $$ = new eve::ast::stmt_expr(yylloc, $1); }
	;

stmt_return
	: RETURN expr_inline EOL							{ $$ = new eve::ast::stmt_return(yylloc, $2); }
	| RETURN expr_block									{ $$ = new eve::ast::stmt_return(yylloc, $2); }
	;

stmt_list 
	: stmt												{ $$ = new std::vector<eve::ast::stmt*>(); $$->push_back($1); }
	| stmt_list stmt									{ $$ = $1; $1->push_back($2); }
	;

stmt_or_block
	: stmt												{ $$ = new std::vector<eve::ast::stmt*>(); $$->push_back($1); }
	| EOL stmt_block									{ $$ = $2; }
	;

stmt_block
	: INDENT stmt_list DEDENT							{ $$ = $2; }
	;

stmt_par
	: '@' optional_perms ':' stmt_or_block				{ $$ = new eve::ast::stmt_par(yylloc, $2, $4); }
	;

stmt_on
	: ON expr_inline SIMPLE_ID	'=' expr_inline EOL		{ $$ = new eve::ast::stmt_on(yylloc, $2, $3, $5); }
	| ON expr_inline SIMPLE_ID	'=' expr_block			{ $$ = new eve::ast::stmt_on(yylloc, $2, $3, $5); }
	;

stmt_finish
	: FINISH ':' stmt_or_block							{ $$ = new eve::ast::stmt_finish(yylloc, $3); }
	;

optional_perms
	: /* EMPTY */							{ $$ = new std::vector<eve::ast::perm*>(); }
	| perms									{ $$ = $1; }
	;

perms
	: '[' ']'								{ $$ = new std::vector<eve::ast::perm*>(); }
	| '[' perm_list ']'						{ $$ = $2; }
	;

perm
	: id									{ $$ = new eve::ast::perm(yylloc, $1, eve::ast::perm_access::access_const); }
	| '&' id								{ $$ = new eve::ast::perm(yylloc, $2, eve::ast::perm_access::access_ref); }
	| '=' id								{ $$ = new eve::ast::perm(yylloc, $2, eve::ast::perm_access::access_copy); }
	| '-' id								{ $$ = new eve::ast::perm(yylloc, $2, eve::ast::perm_access::access_none); }
	| '+' id								{ $$ = new eve::ast::perm(yylloc, $2, eve::ast::perm_access::access_full); }
	;

perm_list
	: perm									{ $$ = new std::vector<eve::ast::perm*>(); $$->push_back($1); }
	| perm_list ',' perm					{ $$ = $1; $1->push_back($3); }
	;

type
	: id %prec LOW_PREC						{ $$ = new eve::ast::type_base(yylloc, $1, new std::vector<eve::ast::argument_type*>()); }
	| type_no_id							{ $$ = $1; }
	;

type_no_id
	: type '@'								{
		std::vector<eve::ast::argument_type*>* arguments = new std::vector<eve::ast::argument_type*>();
		arguments->push_back( new eve::ast::argument_type_type(yyloc, $1));
		$$ = new eve::ast::type_base(yylloc, new eve::ast::simple_id(yyloc, new std::string("shared")), arguments);
	}
	| type '&'								{
		std::vector<eve::ast::argument_type*>* arguments = new std::vector<eve::ast::argument_type*>();
		arguments->push_back( new eve::ast::argument_type_type(yyloc, $1));
		$$ = new eve::ast::type_base(yylloc, new eve::ast::simple_id(yyloc, new std::string("reference")), arguments);
	}
	| '<' '(' ')' type '>'  				{
		std::vector<eve::ast::argument_type*>* arguments = new std::vector<eve::ast::argument_type*>();
		arguments->push_back( new eve::ast::argument_type_type(yyloc, $4));
		$$ = new eve::ast::type_base(yylloc, new eve::ast::simple_id(yyloc, new std::string("function")), arguments);
	}

	| '<' '(' type_list ')' type '>'		{
		std::vector<eve::ast::argument_type*>* arguments = new std::vector<eve::ast::argument_type*>();
		arguments->push_back( new eve::ast::argument_type_type(yyloc, $5));

		for (size_t i = 0; i < $3->size(); i++)
			arguments->push_back(new eve::ast::argument_type_type(yyloc, (*$3)[i]));
		delete $3;

		$$ = new eve::ast::type_base(yylloc, new eve::ast::simple_id(yyloc, new std::string("function")), arguments);
	}
	| type ELIPSIS							{ $$ = new eve::ast::type_elipsis(yyloc, $1); }
	| id '<' argument_type_list '>'			{ $$ = new eve::ast::type_base(yylloc, $1, $3); }
	;

argument_type
	: id									{ $$ = new eve::ast::argument_type_id(yyloc, $1); } /* TODO: check for simple type, or compile time const valued variable */ 
	| type_no_id							{ $$ = new eve::ast::argument_type_type(yyloc, $1); }
	| CONSTANT								{ $$ = new eve::ast::argument_type_constant(yylloc, $1); }
	;

argument_type_list
	: argument_type							{ $$ = new std::vector<eve::ast::argument_type*>(); $$->push_back($1); }
	| argument_type_list ',' argument_type	{ $$ = $1; $1->push_back($3); }
	;

parameter_function
	: SIMPLE_ID								{ $$ = new eve::ast::parameter_function_named(yylloc, NULL, $1); }
	| SIMPLE_ID ':' type 					{ $$ = new eve::ast::parameter_function_named(yylloc, $3, $1); }
	| SELF									{ $$ = new eve::ast::parameter_function_self(yylloc); }
	;

parameter_function_list
	: parameter_function								{ $$ = new std::vector<eve::ast::parameter_function*>(); $$->push_back($1); }
	| parameter_function_list ',' parameter_function	{ $$ = $1; $1->push_back($3); }
	;

optional_parameter_function_list
	: /* EMPTY */							{ $$ = new std::vector<eve::ast::parameter_function*>(); }
	| parameter_function_list				{ $$ = $1; }
	;

module
	: import_list definition_list			{ $$ = new eve::ast::module(yylloc, $1, $2); }
	| definition_list						{ $$ = new eve::ast::module(yylloc, new std::vector<eve::ast::import*>(), $1); }
	;

start
	: module								{ this->driver.add_module(*this->driver.filename(), 	$1); }
	;

 /*** END EVE - Change the eve grammar rules above ***/

%% /*** Additional Code ***/

/* add parser implementations here */

void eve::parser::error(const eve::location& location, const std::string& message)
{
	this->driver.error(location, message);
}
