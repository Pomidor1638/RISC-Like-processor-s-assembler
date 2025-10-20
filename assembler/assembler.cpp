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

bool Assembler::writeFile(std::string output_file)
{
	if (!is_ok()) 
	{
		return false;
	}

	std::ofstream file(output_file, std::fstream::binary);

	if (!file.is_open())
	{
		errors.push_back(
			Error{
				ErrorType::CANNOT_WRITE_FILE,
				"Can't open file " + output_file,
				-1
			}
		);
		return false;
	}

	const size_t size = assembled_instructions.size() * sizeof(instruction_t);
	file.write((char*)assembled_instructions.data(), size);

	if (!file.good()) 
	{
		errors.push_back(
			Error{
				ErrorType::CANNOT_WRITE_FILE,
				"Can't write to file " + output_file,
				-1
			}
		);
		return false;
	}

	return true;

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