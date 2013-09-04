#ifndef EVE_SYMBOL_CONTEXT_IMPL
#define EVE_SYMBOL_CONTEXT_IMPL

namespace eve
{
namespace symbol
{

context::context(eve::symbol::context* parent)
	: parent(parent)
{
}

context::~context()
{
}

std::vector<eve::ast::node*> context::lookup(eve::ast::id* id, bool up) const
{
	std::vector<eve::ast::node*> ret;

	eve::ast::simple_id* simple = dynamic_cast<eve::ast::simple_id*>(id);
	if (simple)
	{
		ret = this->lookup(simple, up);
		return ret;
	}

	eve::ast::qualified_id* qualified = dynamic_cast<eve::ast::qualified_id*>(id);
	if (qualified)
	{
		ret = this->lookup(qualified, up, 0);
		return ret;
	}

	return ret;
}

std::vector<eve::ast::node*> context::lookup(eve::ast::simple_id* id, bool up) const
{
	std::vector<eve::ast::node*> ret;

	for (auto it = this->symbols.equal_range(id->string()); it.first != it.second; ++it.first)
		ret.push_back(it.first->second);

	if (ret.empty() && up && this->parent)
		ret = this->parent->lookup(id, up);

	return ret;
}

std::vector<eve::ast::node*> context::lookup(const std::string& id, bool up) const
{
	std::vector<eve::ast::node*> ret;

	for (auto it = this->symbols.equal_range(id); it.first != it.second; ++it.first)
		ret.push_back(it.first->second);

	if (ret.empty() && up && this->parent)
		ret = this->parent->lookup(id, up);

	return ret;
}

std::vector<eve::ast::node*> context::lookup(eve::ast::qualified_id* id, bool up, size_t pos) const
{
	eve::ast::simple_id* simple = id->ids()[pos];

	std::vector<eve::ast::node*> ret;
	for (auto it = this->symbols.equal_range(simple->string()); it.first != it.second; ++it.first)
		ret.push_back(it.first->second);

	if (ret.empty() && up && this->parent)
	{
		ret = this->parent->lookup(id, up, pos);
		return ret;
	}

	assert(ret.size() <= 1);

	/* finished the entire qualified_id*/
	if (pos == id->ids().size() - 1)
		return ret;

	/* need to look deeper */
	for (std::vector<eve::ast::node*>::iterator it = ret.begin(); it != ret.end(); ++it)
	{
		std::vector<eve::ast::node*> sub = (*it)->context().lookup(id, false, pos + 1);
		std::copy(sub.begin(), sub.end(), ret.begin());
	}

	return ret;
}

void context::insert(eve::ast::simple_id* id, eve::ast::node* node)
{
	this->symbols.emplace(id->string(), node);
}

void context::insert(const std::string& id, eve::ast::node* node)
{
	this->symbols.emplace(id, node);
}

std::string context::string() const
{
	std::string ret = std::to_string((long long) this) + "[";

	for (symbol_table::const_iterator it = this->symbols.begin(); it != this->symbols.end(); ++it)
		ret += it->first + ": " + std::to_string((long long) it->second) + ", ";

	ret = 

	ret += "]";
	return ret;
}

}
}

#endif