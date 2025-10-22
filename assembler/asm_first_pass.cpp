#include "assembler.h"
#include "utils.h"

bool Assembler::first_pass(const std::list<std::string>& lines)
{
	if (verbose)
	{
		std::cout << __func__ << std::endl;
	}

	line_num = 0;

	bool result = true;


	for (const auto& line : lines)
	{


		if (line.empty())
			continue;

		if (verbose)
			std::cout << "Analyze line: "
			<< line_num << "...";


		// YandereDev moment

		if (isLabel(line))
		{
			if (verbose)
				std::cout << "it's label..." << std::endl;
			if (!analyzeLabel(line))
			{
				if (verbose)
					std::cout << "fail" << std::endl;
				result = false;
			}
			else if (verbose)
				std::cout << "success" << std::endl;
		}
		else if (isInstruction(line))
		{
			if (verbose)
				std::cout << "it's instruction...\n";
			if (!analyzeInstruction(line))
			{
				if (verbose)
					std::cout << "fail" << std::endl;
				result = false;
			}
			else if (verbose)
				std::cout << "success" << std::endl;
		}
		else if (isDirective(line))
		{
			if (verbose)
				std::cout << "it's directive...\n";
			if (1)
			{
				if (verbose)
					std::cout << "fail" << std::endl;
				result = false;
			}
			else if (verbose)
				std::cout << "success" << std::endl;
		}
		else
		{
			if (verbose)
				std::cout << "what the fuck is this ?" << std::endl;
			addError(ErrorType::UNEXCEPTED_TOKEN, line, line_num);
			result = false;
		}

		line_num++;
	}

	if (!has_entry_point)
	{
		addError(ErrorType::NO_ENTRY_POINT, "", -1, true);
		result = false;
	}

	return result;
}

bool Assembler::analyzeInstruction(const std::string& line)
{
	bool result = true;
	std::stringstream ss(line);
	std::string opcode;

	ss >> opcode;

	if (!isValidInstruction(line))
	{
		addError(ErrorType::UNEXCEPTED_OPCODE, line, line_num);
		result = false;
	}

	if (curBlock)
	{
		if (result)
			curBlock->size += getOpcodeSize(opcode);
	}
	else
	{
		addError(ErrorType::UNEXCEPTED_OPCODE_PLACEMENT, line, line_num);
		result = false;
	}

	return result;
}


bool Assembler::analyzeDirective(const std::string line)
{
	return false;
}

bool Assembler::analyzeLabel(const std::string& line)
{

	if (verbose)
		std::cout << __func__ << std::endl;

	std::string label_name = line.substr(0, line.length() - 1);

	curBlock = nullptr;

	// Trying ti find duplicate 

	if (block_by_label.find(label_name) != block_by_label.end())
	{

		if (verbose)
			std::cout << "Found duplicate label " << label_name << std::endl;

		addError(ErrorType::MULTIPLE_DEFINITIONS, line, line_num, true);
		return false;
	}

	// Is it ENTRY_POINT ?

	if (isEntryPoint(label_name))
	{

		if (verbose)
			std::cout << "Found entry point " << ENTRY_POINT << std::endl;

		has_entry_point = true;

		blocks.push_front(AssembledBlock{}); // Entry point always first !!!
		curBlock = &blocks.front();
	}
	else
	{
		// No, it's not
		blocks.push_back(AssembledBlock{});
		curBlock = &blocks.back();
	}

	curBlock->label = label_name;
	block_by_label[label_name] = curBlock;

	return true;
}
