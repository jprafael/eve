#ifndef EVE_DRIVER_HH
#define EVE_DRIVER_HH

#include <string>
#include <vector>
#include <map>
#include <queue>

#include "ast/ast.hh"
#include "checker/checker.hh"

// forward declaration
class CalcContext;

/** The eve namespace is used to encapsulate the three parser classes
 * eve::parser, eve::scanner and eve::driver */
namespace eve {

/** The driver class brings together all components. It creates an instance of
 * the parser and scanner classes and connects them. Then the input stream is
 * fed into the scanner object and the parser gets it's token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved. */
class driver
{
	std::string current_path;

	bool _trace_scanning;
	bool _trace_parsing;

	int _errors;
	int _warnings;

	/* only the last position is used but all others must be mantained
	 * because "position"s store a pointer to the strings. */
	std::vector<std::string*> _filename;

	std::queue<std::string> required;
	std::vector<std::string> paths;

	eve::ast::program program;

	friend class eve::visitor::import_resolve_visitor;
	friend class eve::checker::checker;
	std::map<std::string, eve::ast::module*> builtins;

	eve::checker::checker _checker;

protected:
	bool parse_file(const std::string& filename);
	int import(const eve::ast::id* id, std::string& filename, std::string& base_path);

	void load_builtins();


public:
	/* used to find the runtime source code */
	static const std::string BUILTIN_PATH;
	static const std::string LIB_PATH;
	static const std::string RUNTIME_PATH;

	driver(const std::string& current_path);
	~driver();

	/* debuging flags */
	void trace_parsing(bool trace);
	void trace_scanning(bool trace);
	
	/* error messaging */
	void error(const std::string& m);
	void error(const class location& l, const std::string& m);

	void warning(const std::string& m);
	void warning(const class location& l, const std::string& m);

	void debug(const std::string& m);
	void debug(const class location& l, const std::string& m);

	int errors() const;
	int warnings() const;

	/* file IO */
	void require(const std::string& filename);

	void parse();
	void analyse();
	void generate();
	
	/* import */
	int import(const eve::ast::id* id, std::string& filename);

	/* called by lexer once a module is parsed*/
	class scanner* lexer;

	void add_module(const std::string& filename, eve::ast::module* module);
	const std::string* filename() const;

	eve::checker::checker& checker();
};

} // namespace eve

#endif // EVE_DRIVER_H
