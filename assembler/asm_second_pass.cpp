#include "assembler.h"

bool Assembler::second_pass(const std::list<std::string>& lines)
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
			std::cout << "Proccessing line: "
			<< line_num << ":\n\t" << line << std::endl << "...";

		// YandereDev moment

		if (isLabel(line))
		{
			if (verbose)
				std::cout << "it's label..." << std::endl;
			if (!processLabel(line))
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
			if (!processInstruction(line))
			{
				if (verbose)
					std::cout << "fail" << std::endl;
				result = false;
			}
			else if (verbose)
				std::cout << "success" << std::endl;
		}

		line_num++;
	}

	return result;
}

bool Assembler::processNoArgsInstruction(const INSTRUCTION_META& instr)
{
	switch (instr.opcode_num)
	{
	case _NOP:
	case _HLT:
		curBlock->assembled_instructions.push_back(packInstruction(instr.opcode_code, instr.func_code, 0, 0, 0));
		break;
	default:
		addError(ErrorType::UNEXCEPTED_INSTRUCTION,
			" Unsupported no-argument instruction ", line_num);
		return false;
	}

	return true;
}

bool Assembler::processOneArgInstruction(const INSTRUCTION_META& instr, const std::string& arg1)
{
	int regnum;
	if (!isRegister(arg1, regnum))
	{
		addError(ErrorType::UNEXCEPTED_REGISTER, arg1, line_num);
		return false;
	}
	
	switch (instr.opcode_code)
	{
	case _JPR:
	case _JIR:
		curBlock->assembled_instructions.push_back
		(
			packInstruction(instr.opcode_code, instr.func_code, 0, regnum, 0)
		);
		break;
	default:
		addError(ErrorType::UNEXCEPTED_INSTRUCTION,
			" Unsupported one-argument instruction ", line_num);
		return false;
	}

	return true;
}

bool Assembler::processTwoArgsInstruction(
	const INSTRUCTION_META& instr,
	const std::string& arg1,
	const std::string& arg2
)
{
	int 
		reg1num = 0, 
		reg2num_or_value = 0,
		buf = 0;

	int 
		rs = 0, 
		rt = 0, 
		rd = 0;

	if (!isRegister(arg1, reg1num))
	{
		addError(ErrorType::UNEXCEPTED_REGISTER, arg1, line_num);
		return false;
	}

	bool is_imm = false;

	if (isRegister(arg2, buf))
	{
		reg2num_or_value = buf;
	}
	else if (isValue(arg2, buf))
	{
		reg2num_or_value = buf;
		is_imm = true;
	}
	else if (isLabel(arg2))
	{
		auto it = block_by_label.find(arg2);
		if (it != block_by_label.end())
		{
			reg2num_or_value = it->second->base_address;
			is_imm = true;
		}
		else
		{
			addError(ErrorType::UNEXCEPTED_LABEL, " Undefined label: " + arg2, line_num);
			return false;
		}
	}
	else
	{
		addError(ErrorType::UNEXCEPTED_ARGUMENT, arg2, line_num);
		return false;
	}

	switch (instr.opcode_code)
	{
	case _LWI:
		if (!is_imm)
		{
			addError(ErrorType::UNEXCEPTED_ARGUMENT, arg2 + " LWI requires immediate value or label ", line_num);
			return false;
		}

		curBlock->assembled_instructions.push_back(packInstruction(instr.opcode_code, instr.func_code, reg1num, 0, 0));
		curBlock->assembled_instructions.push_back(instruction_t(reg2num_or_value));

		return true;

	case _TCP:
	case _SHL:
	case _SHR:
	case _LWD:
	case _NOT:
	case _MOV:
	case _JRL:

		rd = reg1num;
		rs = reg2num_or_value;

		break;

	case _SWD:

		rt = reg1num;
		rs = reg2num_or_value;
		break;

	case _JGZ:
	case _JLZ:
	case _JEZ:
	case _JNZ:

		rs = reg1num;
		rt = reg2num_or_value;

		break;

	default:
		addError(ErrorType::UNEXCEPTED_INSTRUCTION, 
			" Unsupported two-argument instruction ", line_num);
		return false;
	}

	if (is_imm)
	{
		addError(ErrorType::UNEXCEPTED_ARGUMENT, arg2 +
			" This instruction requires register as second argument ", line_num);
		return false;
	}

	curBlock->assembled_instructions.
	push_back(
		packInstruction(instr.opcode_code, instr.func_code, rd, rs, rt)
	);

	return true;
}

bool Assembler::processThreeArgsInstruction(
	const INSTRUCTION_META& instr,
	const std::string& arg1,
	const std::string& arg2,
	const std::string& arg3
)
{

	int rd, rs, rt; // fortunately all 3 rags funcs using only registers

	struct __arg_check__
	{
		std::function<bool(const std::string&, int&)> func;
		const std::string& arg;
		int& value;

	} checks[] =
	{
		{ isRegister, arg1, rd },
		{ isRegister, arg2, rs },
		{ isRegister, arg3, rt },
	};

	
	for (const auto& x : checks)
	{
		if (!x.func(x.arg, x.value))
		{
			addError(ErrorType::UNEXCEPTED_ARGUMENT, x.arg + " 3-arg instruction requires only registers ", line_num);
			return false;
		}
	}

	switch (instr.opcode_num)
	{
	case _CCT:
	case _ADD:
	case _SUB:
	case _AND:
	case _ORR:
		curBlock->assembled_instructions.push_back(
			packInstruction(instr.opcode_code, instr.func_code, rd, rs, rt)
		);
		break;

	default:
		addError(ErrorType::UNEXCEPTED_INSTRUCTION,
			"Unsupported three-argument instruction", line_num);
		return false;
	}

	return true;
}



bool Assembler::processInstruction(const std::string& line)
{
	if (line.empty() || line.length() < 3)
	{
		addError(ErrorType::UNEXCEPTED_INSTRUCTION, line, line_num);
		return false;
	}

	if (line[line.length() - 1] == ',')
	{
		addError(ErrorType::UNEXCEPTED_INSTRUCTION, line, line_num);
		return false;
	}

	bool result = true;
	auto tokens = parse_instruction(line);

	if (tokens.empty())
	{
		addError(ErrorType::UNEXCEPTED_INSTRUCTION, line, line_num);
		return false;
	}

	INSTRUCTION_META meta;

	std::string opcode;
	std::string args[3];

	opcode = tokens.front();
	tokens.pop_front();

	if (!isOpcode(opcode, meta))
	{
		addError(ErrorType::UNEXCEPTED_OPCODE, line, line_num);
		return false;
	}

	if (meta.opcode_args_num != tokens.size())
	{
		addError(ErrorType::UNEXCEPTED_ARGS_NUM, line, line_num);
		return false;
	}
	
	for (int i = 0; i < meta.opcode_args_num; i++)
	{
		args[i] = tokens.front();
		tokens.pop_front();
	}

	switch (meta.opcode_args_num)
	{
	case 0:
		result = processNoArgsInstruction(meta);
		break;
	case 1:
		result = processOneArgInstruction(meta, args[0]);
		break;
	case 2:
		result = processTwoArgsInstruction(meta, args[0], args[1]);
		break;
	case 3:
		result = processThreeArgsInstruction(meta, args[0], args[1], args[2]);
		break;
	default:
		addError(ErrorType::UNEXCEPTED_ARGS_NUM, line, line_num);
		return false;
	}

	if (!result) 
	{
		addError(ErrorType::UNEXCEPTED_ARGUMENT, line, line_num);
	}

	return result;
}



bool Assembler::processLabel(const std::string& line)
{
	if (line.empty() || line.length() <= 1) 
	{
		addError(ErrorType::UNEXCEPTED_LABEL, line, line_num);
		return false;
	}

	std::string labelname = line.substr(0, line.length() - 1);

	auto it = block_by_label.find(labelname);

	if (it == block_by_label.end())
	{
		addError(ErrorType::UNEXCEPTED_LABEL, line, line_num);
		return false;
	}

	curBlock = it->second;

	return true;
}