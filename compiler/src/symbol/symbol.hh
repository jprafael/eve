#ifndef EVE_SYMBOL_HH
#define EVE_SYMBOL_HH

#include <string>
#include <unordered_map>

namespace eve
{

namespace ast
{
	class node;
	class id;
	class simple_id;
	class qualified_id;
}

namespace visitor
{
	class context_visitor;
	class import_resolve_visitor;
}

namespace checker
{
	class checker;
}

namespace symbol
{

class context
{

public:
	typedef std::unordered_multimap<std::string, eve::ast::node*> symbol_table;

protected:
	eve::symbol::context* parent;
	symbol_table symbols;

	friend eve::ast::node;
	friend eve::visitor::context_visitor;
	friend eve::visitor::import_resolve_visitor;
	friend eve::checker::checker;
	
private:
	context& operator = (const context& other);
	context(const context& other);
	
public: 
	context(eve::symbol::context* parent = NULL);
	virtual ~context();

	std::vector<eve::ast::node*> lookup(const std::string& id, bool up = true) const;
	std::vector<eve::ast::node*> lookup(eve::ast::id* id, bool up = true) const;
	std::vector<eve::ast::node*> lookup(eve::ast::simple_id* id, bool up = true) const;
	std::vector<eve::ast::node*> lookup(eve::ast::qualified_id* id, bool up = true, size_t pos = 0) const;

	void insert(const std::string& id, eve::ast::node* node);
	void insert(eve::ast::simple_id* id, eve::ast::node* node);

	std::string string() const;
};

}
}

#endif