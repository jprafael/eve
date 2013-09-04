namespace eve
{
namespace target
{

std::string task::string() const
{
	std::string ret;

	ret += "class " + this->id;

	if (this->parents.size() > 0)
	{
		ret += ": ";
		for (size_t i = 0; i < this->parents.size(); i++)
		{	
			ret += "public " + parents[i];
			if (i != this->parents.size() -1)
				ret += ", ";
		}
	}

	ret += "\n";
	ret += "{\n";
	ret += "public:\n";

	for (size_t i = 0; i < this->fields.size(); i++)
		ret += "\t" + this->fields[i] + ";\n";

	for (size_t i = 0; i < this->constructors.size(); i++)
		ret += "\t" + this->constructors[i].string() + "\n\n";

	ret += "inline void __pre_exec()\n{\n";
	for (size_t i = 0; i < this->pre_exec.size(); i++)
		ret += "\t" + this->pre_exec[i] + "\n";
	ret += "}\n";

	ret += "inline void __post_exec()\n{\n";
	for (size_t i = 0; i < this->post_exec.size(); i++)
		ret += "\t" + this->post_exec[i] + "\n";
	ret += "}\n";

	for (size_t i = 0; i < this->methods.size(); i++)
		ret += "\t" + this->methods[i].string() + "\n\n";

	ret += "};\n";

	return ret;
}

}
}