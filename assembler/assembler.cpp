#include "assembler.h"
#include "utils.h"


Assembler::Assembler()
	: labels{}
	, pointers{}
	, errors{}
	, assembled_instructions{}
{

}


bool Assembler::is_ok()
{
	return errors.empty();
}


void Assembler::clear()
{
	labels.clear();
	pointers.clear();
	errors.clear();
	assembled_instructions.clear();
}

std::string Assembler::getError() const
{

}

void Assembler::writeFile(std::string output_file)
{

}

bool Assembler::assemble(std::string source_code, bool verbose)
{
	clear();

	const std::list<std::string> lines = split_text_to_lines(source_code);

	int i = 0;

	for (const auto& line : lines)
	{
		if (line.empty())
			continue;



		i++;
	}

}