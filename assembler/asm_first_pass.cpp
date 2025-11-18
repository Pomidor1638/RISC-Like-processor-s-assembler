#include "assembler.h"
#include "utils.h"

bool Assembler::asm_first_pass(const std::list<std::string>& lines)
{
	qprintf(verbose, 2, __func__);

	line_num = 1;
	bool result = true;

	for (const auto& line : lines)
	{
		if (line.empty())
		{
			line_num++;
			continue;
		}

		if (isLabel(line, false))
		{
			if (!analyzeLabel(line))
				result = false;
		}
		else if (isInstruction(line))
		{
			if (!analyzeInstruction(line))
				result = false;
		}
		else if (isDirective(line))
		{
			if (!analyzeDirective(line))
				result = false;
		}
		else
		{
			error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_TOKEN, " What the fuck is this ? " + line, line_num);
			result = false;
		}
		line_num++;
	}

	if (!has_entry_point)
	{
		error_log.addError(ErrorLog::ASSEMBLER_NO_ENTRY_POINT, {}, -1, true);
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
				error_log.addError(ErrorLog::ASSEMBLER_ROM_OVERFLOW, block.label, -1, true);
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

						error_log.addError(ErrorLog::ASSEMBLER_BLOCKS_OVERLAP, ss.str(), -1, true);
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
	qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

	bool result = true;
	std::stringstream ss(line);
	std::string opcode;

	ss >> opcode;

	if (!isValidInstruction(line))
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_INSTRUCTION, line, line_num);
		result = false;
	}

	if (curBlock)
	{
		if (result)
			curBlock->size += getOpcodeSize(opcode);
	}
	else
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_PLACEMENT, line, line_num);
		result = false;
	}

	return result;
}

bool Assembler::analyzeDirectiveString(const std::string line)
{
	qprintf(verbose, 4, "%s\n%s", __func__, line.c_str());

	auto tokens = parse_directiveString(line);

	if (tokens.empty())
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_STRING, line, line_num);
		return false;
	}

	if (!curBlock)
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_PLACEMENT, line, line_num);
		return false;
	}
	std::string str_content = tokens.front();
	int byte_length = str_content.length() + 1;
	int instruction_length = (byte_length + 1) / 2;
	curBlock->size += instruction_length;

	qprintf(verbose, 2, "String: \"%s\", length: %d bytes, instructions: %d",
		str_content.c_str(), byte_length, instruction_length);

	return true;
}

bool Assembler::analyzeDirectiveByte(const std::string line)
{
	qprintf(verbose, 4, "%s\n%s", __func__, line.c_str());
	auto tokens = parse_directiveData(line);
	if (!curBlock)
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_PLACEMENT, line, line_num);
		return false;
	}
	for (const auto& token : tokens)
	{
		int value;
		if (!isValue8(token, value))
		{
			error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_IMM_VALUE, line, line_num);
			return false;
		}
	}
	if (tokens.empty())
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_ARGUMENT, line, line_num);
		return false;
	}
	int instruction_count = (tokens.size() + 1) / 2;
	curBlock->size += instruction_count;
	qprintf(verbose, 3, ".byte: %zu bytes -> %d instructions", tokens.size(), instruction_count);

	return true;
}

bool Assembler::analyzeDirectiveData16(const std::string line)
{
	qprintf(verbose, 4, "%s\n%s", __func__, line.c_str());

	auto tokens = parse_directiveData(line);

	if (!curBlock)
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_PLACEMENT, line, line_num);
		return false;
	}

	for (const auto& token : tokens)
	{
		int value;
		if (!isValue16(token, value))
		{
			error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_IMM_VALUE, line, line_num);
			return false;
		}
	}

	if (tokens.empty())
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_ARGUMENT, line, line_num);
		return false;
	}

	curBlock->size += tokens.size();
	return true;
}

bool Assembler::analyzeDirectiveData32(const std::string line)
{
	qprintf(verbose, 4, "%s\n%s", __func__, line.c_str());

	auto tokens = parse_directiveData(line);

	if (!curBlock)
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_PLACEMENT, line, line_num);
		return false;
	}

	for (const auto& token : tokens)
	{
		int value;
		if (!isValue32(token, value))
		{
			error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_IMM_VALUE, line, line_num);
			return false;
		}
	}

	if (tokens.empty())
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_ARGUMENT, line, line_num);
		return false;
	}

	curBlock->size += tokens.size() * 2;
	return true;
}

bool Assembler::analyzeDirectiveLoadFile(const std::string line)
{
	qprintf(verbose, 4, "%s\n%s", __func__, line.c_str());

	auto tokens = parse_directiveLoadFile(line);

	if (!curBlock)
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_PLACEMENT, line, line_num);
		return false;
	}

	if (tokens.empty())
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_ARGUMENT, line, line_num);
		return false;
	}

	std::string filename = tokens.front();

	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		error_log.addError(ErrorLog::FILE_CANNOT_OPEN, line, line_num);
		return false;
	}

	std::streamsize file_size = file.tellg();
	file.close();

	if (file_size < 0)
	{
		error_log.addError(ErrorLog::FILE_CANNOT_READ, line, line_num);
		return false;
	}

	int instruction_count = (file_size + 1) / 2; // Округление вверх
	curBlock->size += instruction_count;

	qprintf(verbose, 2, "Load file: %s, size: %zd bytes, aligned: %zu bytes",
		filename.c_str(), file_size, instruction_count * 2);

	return true;
}

bool Assembler::analyzeDirective(const std::string line)
{
	qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

	std::stringstream ss(line);
	std::string str_directive;

	ss >> str_directive;

	std::string directive_name = str_directive;
	if (directive_name[0] != '.') 
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_DIRECTIVE, line, line_num);
		return false;
	}

	directive_name = directive_name.substr(1);
	auto it = ASSEMBLER_DIRECTIVES.find(directive_name);
	if (it == ASSEMBLER_DIRECTIVES.end())
	{
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_DIRECTIVE, line, line_num);
		return false;
	}

	AssemblerDirective dir = it->second;

	switch (dir)
	{
	case ASM_BYTE:
		return analyzeDirectiveByte(line);
	case ASM_DATA16:
		return analyzeDirectiveData16(line);
	case ASM_DATA32:
		return analyzeDirectiveData32(line);
	case ASM_STRING:
		return analyzeDirectiveString(line);
	case ASM_INCBIN:
		return analyzeDirectiveLoadFile(line);
	default:
		error_log.addError(ErrorLog::ASSEMBLER_UNEXCEPTED_DIRECTIVE, line, line_num);
		return false;
	}
}

bool Assembler::analyzeLabel(const std::string& line)
{
	qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

	std::string label_name = line.substr(0, line.length() - 1);
	curBlock = nullptr;

	if (block_by_label.find(label_name) != block_by_label.end())
	{
		qprintf(verbose, 0, "Found duplicate label %s\n", label_name.c_str());
		error_log.addError(ErrorLog::ASSEMBLER_MULTIPLE_DEFINITIONS, line, line_num, true);
		return false;
	}

	// Is it ENTRY_POINT ?
	if (isEntryPoint(label_name))
	{
		qprintf(verbose, 0, "Found entry point START");
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
