#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

#include "driver.hh"
#include "scanner.hh"
#include "util.hh"
#include "visitor/visitor.hh"
#include "target/target.hh"

namespace eve {

const std::string driver::BUILTIN_PATH = "/builtin";
const std::string driver::LIB_PATH = "/lib";
const std::string driver::RUNTIME_PATH = "/runtime";

driver::driver(const std::string& current_path)
	: current_path(current_path), _trace_scanning(false), _trace_parsing(false), _errors(0), _warnings(0), _checker(*this)
{
	this->paths.push_back(current_path + driver::LIB_PATH);
}

driver::~driver()
{
	for (size_t i = 0; i < this->_filename.size(); i++)
		delete this->_filename[i];
}

void driver::trace_scanning(bool trace)
{
	this->_trace_scanning = trace;
}

void driver::trace_parsing(bool trace)
{
	this->_trace_parsing = trace;
}

bool driver::parse_file(const std::string &filename)
{
	this->_filename.push_back(new std::string(filename));

	std::ifstream in(filename.c_str());

	if (!in.good())
	{
		this->error(std::string("unable to open file \"") + filename + "\"");
		return false;
	}

	scanner scanner(this, &in);
	scanner.set_debug(this->_trace_scanning);
	this->lexer = &scanner;

	parser parser(*this);
	parser.set_debug_level(this->_trace_parsing);

	return (parser.parse() == 0);
}

void driver::error(const std::string& m)
{
	this->_errors++;
	std::cerr << "error: " << m << std::endl;
}

void driver::error(const class location& l, const std::string& m)
{
	this->_errors++;
	std::cerr << l << " error: " << m << std::endl;
}

void driver::warning(const std::string& m)
{
	this->_warnings++;
	std::cerr << "warning: " << m << std::endl;
}

void driver::warning(const class location& l, const std::string& m)
{
	this->_warnings++;
	std::cerr << l << " warning: " << m << std::endl;
}

void driver::debug(const std::string& m)
{
	std::cerr << "debug: " << m << std::endl;
}

void driver::debug(const class location& l, const std::string& m)
{
	std::cerr << l << " debug: " << m << std::endl;
}

int driver::errors() const
{
	return this->_errors;
}

int driver::warnings() const
{
	return this->_warnings;
}

void driver::require(const std::string& filename)
{
	if (filename[0] == '/')
		this->required.push(filename);

	else
		this->required.push(this->current_path + '/' + filename);
}

int driver::import(const eve::ast::id* id, std::string& filename, std::string& base_path)
{
	std::vector<eve::ast::simple_id*> ids = id->ids();

	for (int j = ids.size() - 1; j >= 0; j--)
	{
		std::string full_path = base_path;
		for (int k = 0; k <= j; k++)
			full_path += "/" + ids[k]->string();

		full_path += ".eve";

		std::ifstream file(full_path);
		if (file)
		{
			filename = full_path;
			this->debug("Found " + id->string() + " at " + full_path);
			return j;
		}
	}

	return -1;
}

int driver::import(const eve::ast::id* id, std::string& filename)
{
	int position;
	std::string current_path = this->filename()->substr(0, this->filename()->rfind("/"));

	/* try current path */
	position = this->import(id, filename, current_path);
	if (position >= 0)
	{
		this->require(filename);
		return position;
	}

	/* try library paths */
	for (int i = this->paths.size() - 1; i >= 0; i--)
	{
		position = this->import(id, filename, this->paths[i]);
		if (position >= 0)
		{
			this->require(filename);
			return position;
		}
	}

	return -1;
};

void driver::parse()
{
	this->debug("Syntatic parsing");

	eve::visitor::import_visitor v(*this);

	while (!this->required.empty())
	{
		std::string filename = this->required.front();
		this->required.pop();

		if (!this->program.context().lookup(filename).empty())
			continue;

		if (this->parse_file(filename))
		{
			std::vector<eve::ast::node*> results = this->program.context().lookup(filename);

			assert(results.size() == 1);
			results[0]->accept(v);
		}
	}
}

void driver::load_builtins()
{
	this->debug("Loading builtins");
	
	std::vector<std::string> files = eve::util::list_directory(this->current_path + driver::BUILTIN_PATH);

	for (size_t i = 0; i < files.size(); i++)
	{
		std::string builtin = this->current_path + driver::BUILTIN_PATH + '/' + files[i];
		this->builtins[builtin] = NULL;
		this->require(builtin);
	}

	/* builtin modules are added to this->builtins in add_module() */
	this->parse();
}

void driver::analyse()
{
	this->debug("Semantic analysis");

	/* prepare */
	this->load_builtins();

	if (this->errors())
		return;

	/* run */
	eve::visitor::context_visitor ctx_visitor(*this);
	this->program.accept(ctx_visitor);
	if (this->errors())
		return;

	eve::visitor::definition_visitor def_visitor(*this);
	this->program.accept(def_visitor);
	if (this->errors())
		return;

	eve::visitor::import_resolve_visitor imp_visitor(*this);
	this->program.accept(imp_visitor);
	if (this->errors())
		return;

	eve::visitor::type_visitor type_visitor(*this);
	this->program.accept(type_visitor);
	if (this->errors())
		return;

	eve::visitor::par_visitor par_visitor(*this);
	this->program.accept(par_visitor);
	if (this->errors())
		return;

	eve::visitor::lambda_visitor lambda_visitor(*this);
	this->program.accept(lambda_visitor);
	if (this->errors())
		return;

	eve::visitor::n_ary_visitor n_visitor(*this);
	this->program.accept(n_visitor);
	if (this->errors())
		return;

	eve::visitor::expr_visitor expr_visitor(*this);
	this->program.accept(expr_visitor);
	if (this->errors())
		return;
}

void driver::generate()
{
	this->debug("Code generation");

	eve::target::cu cu;
	this->program.translate(*this, cu);

	std::cout << cu;
}

void driver::add_module(const std::string& filename, eve::ast::module* module)
{
	this->program.add_module(filename, module);

	if (this->builtins.find(filename) != this->builtins.end())
		this->builtins[filename] = module;
}

const std::string* driver::filename() const
{
	return this->_filename.back();
}

eve::checker::checker& driver::checker()
{
	return this->_checker;
}

} // namespace eve
