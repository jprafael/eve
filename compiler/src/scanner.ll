%{ /*** C/C++ Declarations ***/

#include <string>
#include <queue>
#include "scanner.hh"

/* import the parser's token type into a local typedef */
typedef eve::parser::token token;
typedef eve::parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "EveFlexLexer" */
%option prefix="EVE"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug

/* no support for include files is planned */
%option yywrap nounput 

/* enables the use of start condition stacks */
%option stack

%x ALREADY_INDENTED

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION	yylloc->columns(yyleng);
%}

N8							[0-7]
N10							[0-9]
N16							[0-9a-fA-F]
INT_SPEC					(u|U|l|L)
FLOAT_SPEC					(f|F|l|L)
EXP							(e|E)[+-]?{N10}+
ALPHA						[a-zA-Z\_]
		
WS							[ \t\v\f\r]
CHAR						(\\(b|t|n|r|\"|\'|\\|u{N16}{4}|{N8}{1,3})|[^\\\"\n])
STRING						\"{CHAR}*\"
COMMENT						\#.*

DECIMAL						(({N10}*"."{N10}+{EXP}?{FLOAT_SPEC}*)|({N10}+{EXP}{FLOAT_SPEC}*))
INTEGER						((0{N8}+{INT_SPEC}*)|({N10}+{INT_SPEC}*)|(0(x|X){N16}+{INT_SPEC}*))

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
	// reset location
	yylloc->step();
%}

 /*** BEGIN EVE - Change the eve lexer rules below ***/

 /* ignore empty lines */
<INITIAL>^{WS}*{COMMENT}?\n? {
	yylloc->lines(1);
	yylloc->step();
	BEGIN(INITIAL);
}

<INITIAL>^[ \t]*[^ \t] {
	eve::scanner* s = static_cast<eve::scanner*>(this);

	int indent = s->indent(yylloc, std::string(yytext, yyleng));

	unput(yytext[yyleng-1]);

	BEGIN(ALREADY_INDENTED);

	if (indent < 0)
		return token::DEDENT;

	else if (indent > 0)
		return token::INDENT;
}

<ALREADY_INDENTED>{INTEGER}	{
	yylval->constant = new eve::ast::constant
	(
		*yylloc,
		new std::string(yytext, yyleng),
		eve::ast::constant_type::type_int
	);
	return token::CONSTANT;
}

<ALREADY_INDENTED>{DECIMAL}	{
	yylval->constant = new eve::ast::constant
	(
		*yylloc,
		new std::string(yytext, yyleng),
		eve::ast::constant_type::type_float
	);
	return token::CONSTANT;
}

<ALREADY_INDENTED>\'{CHAR}\' {
	yylval->constant = new eve::ast::constant
	(
		*yylloc,
		new std::string(yytext, yyleng),
		eve::ast::constant_type::type_char
	);
	return token::CONSTANT;
}

<ALREADY_INDENTED>{STRING} {
	yylval->constant = new eve::ast::constant
	(
		*yylloc,
		new std::string(yytext, yyleng),
		eve::ast::constant_type::type_string
	);
	return token::CONSTANT;
}

<ALREADY_INDENTED>("false")|("true") {
	yylval->constant = new eve::ast::constant
	(
		*yylloc,
		new std::string(yytext, yyleng),
		eve::ast::constant_type::type_bool
	);
	return token::CONSTANT;
}

<ALREADY_INDENTED>\n {
	yylloc->lines(yyleng); yylloc->step();
	BEGIN(INITIAL);
	return token::EOL;
}

<ALREADY_INDENTED>{COMMENT} {
	yylloc->step();
}

 /* gobble up white-spaces */
<INITIAL,ALREADY_INDENTED>{WS}+ {
	yylloc->step();
}

<ALREADY_INDENTED>"import"				{ return token::IMPORT; }
<ALREADY_INDENTED>"as"					{ return token::AS; }
<ALREADY_INDENTED>"builtin"				{ return token::BUILTIN; }

<ALREADY_INDENTED>"type"				{ return token::TYPE; }
<ALREADY_INDENTED>"var"					{ return token::VAR; }
<ALREADY_INDENTED>"self"				{ return token::SELF; }
<ALREADY_INDENTED>"operator"			{ return token::OPERATOR; }
<ALREADY_INDENTED>"constructor"			{ return token::CONSTRUCTOR; }

<ALREADY_INDENTED>"if"					{ return token::IF; }
<ALREADY_INDENTED>"else"				{ return token::ELSE; }
<ALREADY_INDENTED>"for"					{ return token::FOR; }
<ALREADY_INDENTED>"while"				{ return token::WHILE; }
<ALREADY_INDENTED>"return"				{ return token::RETURN; }
<ALREADY_INDENTED>"pass"				{ return token::PASS; }
<ALREADY_INDENTED>"finish"				{ return token::FINISH; }

<ALREADY_INDENTED>"in"					{ return token::IN; }
<ALREADY_INDENTED>"on"					{ return token::ON; }

<ALREADY_INDENTED>"..."					{ return token::ELIPSIS; }

 /*<ALREADY_INDENTED>"break"			{ return token::BREAK; }
<ALREADY_INDENTED>"case"				{ return token::CASE; }
<ALREADY_INDENTED>"continue"			{ return token::CONTINUE; }
<ALREADY_INDENTED>"default"				{ return token::DEFAULT; }
<ALREADY_INDENTED>"do"					{ return token::DO; }
<ALREADY_INDENTED>"final"				{ return token::FINAL; }
<ALREADY_INDENTED>"new"					{ return token::NEW; }
<ALREADY_INDENTED>"private"				{ return token::PRIVATE; }
<ALREADY_INDENTED>"public"				{ return token::PUBLIC; }
<ALREADY_INDENTED>"static"				{ return token::STATIC; }
<ALREADY_INDENTED>"switch"				{ return token::SWITCH; }
*/

 /* identifiers */
<ALREADY_INDENTED>{ALPHA}({ALPHA}|{N10})* {
	yylval->simple_id = new eve::ast::simple_id(*yylloc, new std::string(yytext, yyleng));
	return token::SIMPLE_ID;
}

<ALREADY_INDENTED>">>="					{ return token::ASSIGN_SHIFT_R;} /* OPERATORS */
<ALREADY_INDENTED>"<<="					{ return token::ASSIGN_SHIFT_L; }
<ALREADY_INDENTED>"+="					{ return token::ASSIGN_ADD; }
<ALREADY_INDENTED>"-="					{ return token::ASSIGN_SUB; }
<ALREADY_INDENTED>"*="					{ return token::ASSIGN_MUL; }
<ALREADY_INDENTED>"/="					{ return token::ASSIGN_DIV; }
<ALREADY_INDENTED>"%="					{ return token::ASSIGN_MOD; }
<ALREADY_INDENTED>"&="					{ return token::ASSIGN_AND; }
<ALREADY_INDENTED>"&&="					{ return token::ASSIGN_BOOL_AND; }
<ALREADY_INDENTED>"|="					{ return token::ASSIGN_OR; }
<ALREADY_INDENTED>"||="					{ return token::ASSIGN_BOOL_OR; }
<ALREADY_INDENTED>"^="					{ return token::ASSIGN_XOR; }
<ALREADY_INDENTED>"**="					{ return token::ASSIGN_POWER; }

<ALREADY_INDENTED>">>"					{ return token::OP_SHIFT_R; }
<ALREADY_INDENTED>"<<"					{ return token::OP_SHIFT_L; }

<ALREADY_INDENTED>"++"					{ return token::OP_INC; }
<ALREADY_INDENTED>"--"					{ return token::OP_DEC; }
<ALREADY_INDENTED>"&&"					{ return token::OP_BOOL_AND; }
<ALREADY_INDENTED>"||"					{ return token::OP_BOOL_OR; }
<ALREADY_INDENTED>"<="					{ return token::OP_LE; }
<ALREADY_INDENTED>">="					{ return token::OP_GE; }
<ALREADY_INDENTED>"==="					{ return token::OP_EQ3; }
<ALREADY_INDENTED>"!=="					{ return token::OP_NE3; }
<ALREADY_INDENTED>"=="					{ return token::OP_EQ; }
<ALREADY_INDENTED>"!="					{ return token::OP_NE; }
<ALREADY_INDENTED>"**"					{ return token::OP_POWER; }

<<EOF>>		 							{ 
	std::queue<eve::parser::token_type> tokens;

	eve::scanner* s = static_cast<eve::scanner*>(this);

	if (YYSTATE == ALREADY_INDENTED)
		tokens.push(token::EOL);

	int indent = s->indent(yylloc, std::string(""));
	if (indent < 0)
		tokens.push(token::DEDENT);

	tokens.push(token::END);

	eve::parser::token_type tok = tokens.front();
	tokens.pop();

	while (!tokens.empty())
	{
		s->enqueue(tokens.front());
		tokens.pop();
	};

	return tok;
}

<ALREADY_INDENTED>.							 { return static_cast<token_type>(*yytext); }

%% /*** Additional Code ***/

#include "driver.hh"

namespace eve {

scanner::scanner(eve::driver* driver, std::istream* in, std::ostream* out)
	: EVEFlexLexer(in, out), driver(driver), _indent(0)
{
}

scanner::~scanner()
{
}

void scanner::set_debug(bool b)
{
	yy_flex_debug = b;
}

int scanner::indent(eve::parser::location_type* yylloc, std::string whitespace)
{
	int old_indent = this->_indent;

	int count = 0;
	bool has_spaces = false;

	for (size_t i = 0; i < whitespace.length(); i++)
	{
		if (whitespace[i] == '\t')
			count++;

		else if (whitespace[i] == ' ')
			has_spaces = true;
	}

	if (has_spaces)
		this->driver->warning(*yylloc, "spaces found in indentation");

	this->_indent = count;

	int diff = this->_indent - old_indent;
	if (diff == 0)
		return 0;

	if (diff > 0)
	{
		while (diff > 1)
		{
			this->queue.push(token::INDENT);
			diff--;
		}

		return 1;
	} else
	{
		while (diff < -1)
		{
			this->queue.push(token::DEDENT);
			diff++;
		}
		
		return -1;
	}
}

void eve::scanner::enqueue(eve::parser::token_type token)
{
	this->queue.push(token);
}

eve::parser::token_type eve::scanner::lex(eve::parser::semantic_type* yylval, eve::parser::location_type* yylloc)
{
	eve::parser::token_type token;

	if (!this->queue.empty())
	{
		token = this->queue.front();
		this->queue.pop();
	} else
		token = this->_lex(yylval, yylloc);

	return token;
}

}

/* This implementation of EveFlexLexer::yylex() is required to fill the
 * vtable of the class EveFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int EVEFlexLexer::yylex()
{
	std::cerr << "in EVEFlexLexer::yylex() !" << std::endl;
	return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int EVEFlexLexer::yywrap()
{
	return 1;
}
