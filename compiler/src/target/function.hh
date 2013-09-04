namespace eve
{
namespace target
{

std::string function::string() const
{
	std::string ret;

	ret = "inline ";
	ret += this->type + " " + this->name + "(";

	for (size_t i = 0; i < this->parameters.size(); i++)
	{
		ret += this->parameters[i];

		if (i != this->parameters.size() - 1)
			ret += ", ";
	}

	ret += ")\n";

	if (this->assigns.size() > 0)
	{
		ret += "\t: ";

		for (size_t i = 0; i < this->assigns.size(); i++)
		{
			ret += this->assigns[i];

			if (i != this->assigns.size() -1)
				ret += ", ";			
		}

		ret += "\n";
	}

	ret += "{\n";

	for (size_t i = 0; i < this->stmts.size(); i++)
		ret += "\t" + this->stmts[i] + "\n";

	ret += "}\n";

	return ret;
}

}
}