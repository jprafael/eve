namespace eve
{
namespace target
{

eve::target::cu::cu()
	: task_id(1)
{
}

std::ostream& operator << (std::ostream& out, const eve::target::cu& cu)
{
	for (size_t i = 0; i < cu.imports.size(); i++)
		out << cu.imports[i];

	out << std::endl;

	for (size_t i = 0; i < cu.type_forwards.size(); i++)
		out << cu.type_forwards[i];

	out << std::endl;

	for (size_t i = 0; i < cu.function_forwards.size(); i++)
		out << cu.function_forwards[i];

	out << std::endl;

	for (size_t i = 0; i < cu.variable_declarations.size(); i++)
		out << cu.variable_declarations[i];

	out << std::endl;

	for (size_t i = 0; i < cu.type_declarations.size(); i++)
		out << cu.type_declarations[i];

	out << std::endl;

	for (size_t i = 0; i < cu.function_declarations.size(); i++)
		out << cu.function_declarations[i];

	out << std::endl;

	for (size_t i = 0; i < cu.main_declaration.size(); i++)
		out << cu.main_declaration[i];

	out << std::endl;
	return out;
}

}
}