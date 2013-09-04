#ifndef EVE_TARGET_HH 
#define EVE_TARGET_HH

#include <iostream>
#include <deque>

namespace eve
{
namespace target
{

struct function;

struct cu
{
	std::vector<std::string> imports;
	std::vector<std::string> type_forwards;
	std::vector<std::string> function_forwards;
	std::vector<std::string> variable_declarations;
	std::vector<std::string> type_declarations;
	std::vector<std::string> function_declarations;
	std::vector<std::string> main_declaration;
	
	int task_id;

	cu();

	friend std::ostream& operator << (std::ostream& out, const eve::target::cu& cu);
};

struct task
{
	std::string id;
	std::vector<std::string> parents;
	std::vector<std::string> fields;
	std::vector<eve::target::function> constructors;
	std::vector<eve::target::function> methods;
	std::deque<std::string> pre_exec;
	std::deque<std::string> post_exec;

	std::string string() const;
};

struct function
{
	std::string type;
	std::string name;
	std::vector<std::string> parameters;
	std::vector<std::string> assigns;
	std::vector<std::string> stmts;

	std::string string() const;
};

}
}

#endif 