#include "assembler.h"
#include "utils.h"

Assembler::Assembler()
	: labels{}
	, pointers{}
	, errors{}
	//, assembled_instructions{}
	, has_entry_point(false)
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
	//assembled_instructions.clear();
	has_entry_point = false;
	line_num = 0;
}

std::string Assembler::getError() const
{

}


void Assembler::addError(ErrorType type, std::string contents, int line)
{
	errors.push_back(Error{ type, contents, -1 });
}


void Assembler::processInstruction(const std::string& line)
{
	std::list<std::string> tokens = parse_line(line);

	std::string opcode, reg, arg1, arg2;

	opcode = tokens.front();
	tokens.pop_back();

	OPCODE_META meta;

	if (!isOpcode(opcode, meta))
	{
		addError(ErrorType::UNEXCEPTED_OPCODE, line, line_num);
		return;
	}

	switch (meta.opcode_args_num)
	{
	case 0:
		// NOP, HLT
		//processNoArgInstruction(meta, 0, 0, 0);
		break;

	case 1:
		// JPR rs 
		// JIR rs
		// etc.
		if (tokens.size() >= 1) {
			std::string arg1 = tokens.front();
			tokens.pop_front();
			//processSingleArgInstruction(meta, arg1, line);
		}
		break;
	case 2:
		// LWI rd, <I>
		// MOV rd, rs  
		// TCP rd, rs 
		// JNE rs, rt
		// etc.
		if (tokens.size() >= 2) 
		{
			std::string arg1 = tokens.front(); tokens.pop_front();
			std::string arg2 = tokens.front(); tokens.pop_front();
			//processTwoArgInstruction(meta, arg1, arg2, line);
		}
		break;

	case 3:
		// ADD rd, rs, rt
		// SUB rd, rs, rt
		// etc.
		if (tokens.size() >= 3) 
		{
			std::string arg1 = tokens.front(); tokens.pop_front();
			std::string arg2 = tokens.front(); tokens.pop_front();
			std::string arg3 = tokens.front(); tokens.pop_front();
			//processThreeArgInstruction(meta, arg1, arg2, arg3, line);
		}
		break;

	default:
		addError(ErrorType::UNEXCEPTED_ARGS_NUM, line, line_num);
		break;
	}
}
void Assembler::processLabel(const std::string& line)
{
	if (isEntryPoint(line))
	{
		if (has_entry_point)
			addError(ErrorType::MULTIPLE_DEFENITIONS, line, line_num);
		else
			has_entry_point = true;
	}
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
		addError(
			ErrorType::CANNOT_OPEN_FILE,
			"Can't open file " + output_file,
			-1
		);
		return false;
	}

	const size_t size = assembled_instructions.size() * sizeof(instruction_t);
	file.write((char*)assembled_instructions.data(), size);



	if (!file.good()) 
	{
		addError(
			ErrorType::CANNOT_WRITE_FILE,
			"Can't write to file " + output_file,
			-1
		);
		return false;
	}

	return true;

}

bool Assembler::assemble(std::string source_code, bool verbose)
{
	clear();

	const std::list<std::string> lines = split_text_to_lines(source_code, true);

	line_num = 0;

	for (const auto& line : lines)
	{
		if (line.empty())
			continue;

		if (isLabel(line))
		{
			processLabel(line);
		}
		else 
		{
			processInstruction(line);
		}


		line_num++;
	}

	if (!has_entry_point)
	{
		addError(ErrorType::NO_ENTRY_POINT, "There is no entry point \'START\'", -1);
	}

}

