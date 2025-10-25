#include "assembler.h"
#include "utils.h"

bool Assembler::first_pass(const std::list<std::string>& lines)
{
	if (verbose)
	{
		std::cout << __func__ << std::endl;
	}

	line_num = 1;

	bool result = true;


	for (const auto& line : lines)
	{
		if (line.empty())
		{
			line_num++;
			continue;
		}

		if (verbose)
			std::cout << "Analyze line: "
			<< line_num <<":\n\t" << line << std::endl << "...";


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
			if (!analyzeDirective(line))
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

	if (result)
	{
		address_t cur_address = 0;

		for (auto& block : blocks)
		{
			block.base_address = cur_address;
			cur_address += block.size;

			if (cur_address >= ROM_SIZE)
			{
				addError(ErrorType::ROM_OVERFLOW, block.label, -1, true);
				result = false;
				break;
			}
		}


		total_size = cur_address;

		if (blocks.size() > 1)
		{
			for (auto it1 = blocks.begin(); it1 != blocks.end(); ++it1)
			{
				const Block& b1 = *it1;

				if (b1.size <= 0)
					continue;

				for (auto it2 = std::next(it1); it2 != blocks.end(); ++it2)
				{
					const Block& b2 = *it2;

					if (b2.size <= 0)
						continue;

					if (is_intersect(b1.base_address, b1.size, b2.base_address, b2.size))
					{
						std::stringstream ss;
						ss << "  " << b1.label << ": 0x" << std::hex << b1.base_address
							<< " - 0x" << b1.base_address + b1.size - 1 << std::endl
							<< "  " << b2.label << ": 0x" << std::hex << b2.base_address
							<< " - 0x" << b2.base_address + b2.size - 1;

						addError(ErrorType::ASSEMBLE_BLOCKS_OVERLAP, ss.str(), -1, true);
						result = false;
					}
				}
			}
		}
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
		addError(ErrorType::UNEXCEPTED_INSTRUCTION, line, line_num);
		result = false;
	}

	if (curBlock)
	{
		if (result)
			curBlock->size += getOpcodeSize(opcode);
	}
	else
	{
		addError(ErrorType::UNEXCEPTED_INSTRUCTION_PLACEMENT, line, line_num);
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

		blocks.push_front(Block{}); // Entry point always first !!!
		curBlock = &blocks.front();
	}
	else
	{
		// No, it's not
		blocks.push_back(Block{});
		curBlock = &blocks.back();
	}

	curBlock->label = label_name;
	block_by_label[label_name] = curBlock;

	return true;
}
