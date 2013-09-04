#ifndef EVE_TYPE_VISITOR_IMPL
#define EVE_TYPE_VISITOR_IMPL

#include <sstream>
#include <limits>

#include "visitor.hh"
#include "../driver.hh"

namespace eve
{
namespace visitor
{

type_visitor::type_visitor(eve::driver& driver)
	: visitor(true), driver(driver)
{
}

type_visitor::~type_visitor()
{
}

void type_visitor::handle(eve::ast::type_base* node)
{
	size_t min_arguments, max_arguments;

	std::vector<eve::ast::node*> nodes = node->context().lookup(node->id);
	if (nodes.empty())
	{
		this->driver.error(node->location(), "failed to locate base type \"" + node->id->string() + "\"");
		return;
	}

	std::stringstream messages;

	for (size_t n = 0; n < nodes.size(); n++)
	{
		bool error = false;
		eve::ast::node* temp = nodes[n];

		eve::ast::definition_type* decl = dynamic_cast<eve::ast::definition_type*>(temp);
		if (!decl)
		{
			eve::ast::parameter_type_type* param = dynamic_cast<eve::ast::parameter_type_type*>(temp);

			if (!param)
			{
				messages << temp->location() << " expected \"" << node->id->string() << "\" to be a type\n";

				error = true;
				goto next_node;
			}

			if (node->arguments->size() > 0)
			{
				messages << temp->location() << " unexpected arguments to parameter type\n";
				error = true;
			}

			goto next_node;
		}

		min_arguments = decl->parameters->size() - decl->defaults;
		max_arguments = decl->parameters->size();

		/* check for variadic templates **/
		if (max_arguments > 0)
		{
			eve::ast::parameter_type* last = (*decl->parameters)[decl->parameters->size() - 1];
			eve::ast::parameter_type_type* last_type = dynamic_cast<eve::ast::parameter_type_type*>(last);
			eve::ast::parameter_type_constant* last_constant = dynamic_cast<eve::ast::parameter_type_constant*>(last);

			if (!last_type && !last_constant)
			{
				messages << temp->location() << " last parameter is neither a type nor a costant\n";
				error = true;
			}
						
			if ((last_type && last_type->elipsis) || (last_constant && last_constant->elipsis))
			{
				min_arguments--;
				max_arguments = std::numeric_limits<size_t>::max();
			}
		}

		if (node->arguments->size() < min_arguments || node->arguments->size() > max_arguments)
		{
			messages
				<< temp->location()
				<< " wrong number of type arguments (got "
				<< std::to_string(node->arguments->size())
				<< " expected "
				<< std::to_string(min_arguments)
				<< "-"
				<< std::to_string(max_arguments)
				<< ")\n";

			error = true;
			goto next_node;
		}

		for (size_t offset = 0; offset <= decl->defaults; offset++)
		{
			/* 
				types may use some number of defaults
				need to check how many to align the parameters and arguments
			*/

			for (size_t i = 0; i < node->arguments->size(); i++)
			{
				/* FIXME: types with default values and ellipsis */
				assert(i + offset < decl->parameters->size());

				if (dynamic_cast<eve::ast::parameter_type_type*>((*decl->parameters)[i+offset]))
				{
					/* if it is a type, the argument can be a type, or an id (that refers to a type) */
					if (dynamic_cast<eve::ast::argument_type_type*>((*node->arguments)[i]))
						continue; /* don't need further checking because the node will also be visited */

					if (dynamic_cast<eve::ast::argument_type_constant*>((*node->arguments)[i]))
					{
						messages
							<< temp->location() 
							<< " expected a type argument, but received a constant for parameter "
							<< std::to_string(i)
							<< "\n";

						error = true;
						continue;
					}

					eve::ast::argument_type_id* id = dynamic_cast<eve::ast::argument_type_id*>((*node->arguments)[i]);
					assert(id);

					/* lookup the id and make sure it refers to a type */
					std::vector<eve::ast::node*> defs = node->context().lookup(id->id);
					if (defs.empty())
					{
						messages << temp->location() << " unknown type \"" << id->id->string() << "\"";
						error = true;
						continue;
					}

					/* FIXME: a parameter type could also have more than one definition (vector<int>/vector<int<8>>/...) */
					assert(defs.size() == 1);
					eve::ast::node* def = defs[0];

					if (!dynamic_cast<eve::ast::definition_type*>(def) && !dynamic_cast<eve::ast::parameter_type_type*>(def))
					{
						messages << "expected a type argument, but \"" << id->id->string() << "\" is not a type\n";
						error = true;
						continue;
					}

				} else if (dynamic_cast<eve::ast::parameter_type_constant*>((*decl->parameters)[i+offset]))
				{
					/* if it is a constant, the argument must be a constant, or constant valued variable (id) */
					if (dynamic_cast<eve::ast::argument_type_constant*>((*node->arguments)[i]))
						continue;

					if (dynamic_cast<eve::ast::argument_type_type*>((*node->arguments)[i]))
					{
						messages << "expected a constant argument, but received a type for parameter " + std::to_string(i) << "\n";
						error = true;
						continue;
					}

				} else
				{
					messages << "parameter " << std::to_string(i) << " is neither a type nor a costant\n";
					error = true;
					goto next_node;
				}

			}

			/* all ok */
			goto next_node;
		}

		messages << temp->location() << " failed to align arguments to parameters\n"; 

next_node:
		if (!error)
		{
			node->decl = temp;
			return;
		}
	}

	this->driver.error(node->location(), std::string("type ") + node->id->string() + " doesn't match any declaration:\n" + messages.str());
}

}
}

#endif
