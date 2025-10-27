#include "assembler.hpp"

bool Assembler::second_pass(const std::list<std::string>& lines)
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
		// YandereDev moment
		if (isLabel(line, false))
		{
			if (!processLabel(line))
				result = false;
		}
		else if (isInstruction(line))
		{
			if (!processInstruction(line))
				result = false;
			
		}
		//else if (isDirective(line))
		//{
		//	/*skip*/
		//	qprintf(verbose, 3, "skip directives now\n");
		//} 
		else
		{
			// I don't why I put this after first pass
			addError(ErrorType::UNEXCEPTED_TOKEN, line, line_num);
			result = false;
		}

		line_num++;
	}

	return result;
}

bool Assembler::processNoArgsInstruction(const INSTRUCTION_META& instr)
{
	switch (instr.opcode_code)
	{
	case OPCODE_NOP:
	case OPCODE_HLT:
		curBlock->assembled_instructions.push_back(packInstruction(instr.opcode_code, 0, 0, 0));
		break;
	default:
		addError(ErrorType::UNEXCEPTED_INSTRUCTION, " Unsupported no-argument instruction ", line_num);
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
	case OPCODE_JMP:
	case OPCODE_JPR:
	case OPCODE_JIR:
	case OPCODE_JPC:
	case OPCODE_JOV:
	case OPCODE_JZD:
		curBlock->assembled_instructions.push_back(packInstruction(instr.opcode_code, 0, regnum, 0));
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
) {
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
	else if (isLabel(arg2, true))
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
	case OPCODE_LWI:
		if (!is_imm)
		{
			addError(ErrorType::UNEXCEPTED_ARGUMENT, arg2 + " LWI requires immediate value or label ", line_num);
			return false;
		}

		curBlock->assembled_instructions.push_back(packInstruction(instr.opcode_code, reg1num, 0, 0));
		curBlock->assembled_instructions.push_back(instruction_t(reg2num_or_value));

		return true;

	case OPCODE_TCP:
	case OPCODE_INC:
	case OPCODE_DEC:
	case OPCODE_LWD:
	case OPCODE_NOT:
	case OPCODE_MOV:
	case OPCODE_JRL:

		rd = reg1num;
		rs = reg2num_or_value;

		break;

	case OPCODE_SWD:

		rt = reg1num;
		rs = reg2num_or_value;
		break;

	case OPCODE_JGZ:
	case OPCODE_JLZ:
	case OPCODE_JEZ:
	case OPCODE_JNZ:

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
		packInstruction(instr.opcode_code, rd, rs, rt)
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

	switch (instr.opcode_code)
	{
	case OPCODE_ADD:
	case OPCODE_SUB:
	case OPCODE_ADC:
	case OPCODE_SBB:
	case OPCODE_MUL:
	case OPCODE_DIV:
	case OPCODE_UML:
	case OPCODE_UDV:
	case OPCODE_MHL:
	case OPCODE_MLH:
	case OPCODE_MHH:
	case OPCODE_MLL:
	case OPCODE_SLL:
	case OPCODE_SRL:
	case OPCODE_SRA:
	case OPCODE_AND:
	case OPCODE_ORR:
		curBlock->assembled_instructions.push_back(
			packInstruction(instr.opcode_code, rd, rs, rt)
		);
		break;

	default:
		addError(ErrorType::UNEXCEPTED_INSTRUCTION, " Unsupported three-argument instruction ", line_num);
		return false;
	}

	return true;
}



bool Assembler::processInstruction(const std::string& line)
{
	qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

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
		qprintf(verbose, 4, "processNoArgsInstruction %s", line.c_str());
		result = processNoArgsInstruction(meta);
		break;
	case 1:
		qprintf(verbose, 4, "processOneArgInstruction %s", line.c_str());
		result = processOneArgInstruction(meta, args[0]);
		break;
	case 2:
		qprintf(verbose, 4, "processTwoArgsInstruction %s", line.c_str());
		result = processTwoArgsInstruction(meta, args[0], args[1]);
		break;
	case 3:
		qprintf(verbose, 4, "processThreeArgsInstruction %s", line.c_str());
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
	qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());
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