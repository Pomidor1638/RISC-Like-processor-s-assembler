#include "assembler.h"
#include "utils.h"

Assembler::Assembler()
	: blocks{}
	, block_by_label{}
	, curBlock(nullptr)
	, errors{}
	, has_entry_point(false)
	, line_num(0)
	, verbose(0)
	, ROM_SIZE(0)
	, total_size(0)
{

}


bool Assembler::is_ok()
{
	return errors.empty();
}

void Assembler::clear()
{
	blocks.clear();
	block_by_label.clear();
	curBlock = nullptr;
	errors.clear();
	has_entry_point = false;
	line_num = 0;
	verbose = 0;
	ROM_SIZE = 0;
	total_size = 0;
}

std::string Assembler::getError() const
{
	std::list<Error> crit_errs{};
	std::stringstream ss;

	int crit_errorsnum = 0;
	int errorsnum = 0;

	// Проходим по всем ошибкам
	for (const auto& x : errors)
	{
		if (x.critical)
		{
			crit_errs.push_back(x);
			crit_errorsnum++;
		}
		else
		{
			if (x.line != -1)
				ss << "Error at line " << x.line << ": ";
			else
				ss << "Error: ";

			ss << getStrByErrorType(x.type)
				<< " - " << x.contents << std::endl;

			errorsnum++;
		}
	}

	for (const auto& x : crit_errs)
	{
		ss << CRITICAL_ERROR_HEADER;
		if (x.line != -1)
			ss << "Critical error at line " << x.line << ": ";
		else
			ss << "Critical error: ";

		ss << getStrByErrorType(x.type)
			<< " - " << x.contents << std::endl;
	}

	ss << std::endl
		<< "==================== ERROR SUMMARY ====================" << std::endl
		<< "Total errors: " << (errorsnum + crit_errorsnum) << std::endl
		<< "  - Critical errors: " << crit_errorsnum << std::endl
		<< "  - Normal errors  : " << errorsnum << std::endl
		<< "=======================================================" << std::endl;

	return ss.str();
}


void Assembler::addError(ErrorType type, std::string contents, int line, bool critical)
{
	Error err = { type, contents, line, critical };

	if (verbose)
	{
		if (critical)
			std::cerr << CRITICAL_ERROR_HEADER;

		if (line != -1)
		{
			std::cerr << "Error at line " << line << ": ";
		}
		else
		{
			std::cerr << "Error: ";
		}

		std::string str_error = getStrByErrorType(type);

		std::cerr << str_error << " - " << contents << std::endl;
	}



	errors.push_back(err);


}

std::string Assembler::getStrByErrorType(ErrorType t) const
{
	static const std::map<ErrorType, std::string> error_messages =
	{
		{ ErrorType::UNEXCEPTED_INSTRUCTION, "Unexpected opcode" },
		{ ErrorType::UNEXCEPTED_OPCODE, "Unexpected opcode" },
		{ ErrorType::UNEXCEPTED_INSTRUCTION_PLACEMENT, "Unexpected instruction placement" },
		{ ErrorType::UNEXCEPTED_LABEL, "Unexpected label" },
		{ ErrorType::UNEXCEPTED_ARGS_NUM, "Unexpected number of arguments" },
		{ ErrorType::UNEXCEPTED_REGISTER, "Unexpected register" },
		{ ErrorType::UNEXCEPTED_ARGUMENT, "Unexpected argument" },
		{ ErrorType::UNEXCEPTED_IMM_VALUE, "Unexpected immediate value" },
		{ ErrorType::UNEXCEPTED_DIRECTIVE, "Unexpected directive" },
		{ ErrorType::UNEXCEPTED_MACRO, "Unexpected macro" },
		{ ErrorType::UNEXCEPTED_TOKEN, "Unexpected token" },

		{ ErrorType::CANNOT_OPEN_FILE, "Cannot open file" },
		{ ErrorType::CANNOT_READ_FILE, "Cannot read file" },
		{ ErrorType::CANNOT_WRITE_FILE, "Cannot write file" },

		{ ErrorType::MULTIPLE_DEFINITIONS, "Multiple definitions" },
		{ ErrorType::MULTIPLE_UNEXCEPTED_ARGS, "Multiple unexcepted arguments" },

		{ ErrorType::ASSEMBLE_BLOCKS_OVERLAP, "Blocks overlap" },

		{ ErrorType::ROM_OVERFLOW, "ROM overflow" },
		{ ErrorType::NO_ENTRY_POINT, "No entry point found" }
	};

	auto it = error_messages.find(t);
	if (it != error_messages.end()) {
		return it->second;
	}
	return "What the fuck is this ErrorType?";
}

std::string Assembler::readFile(const std::string& filename)
{
	qprintf(verbose, 1, __func__);

	std::ifstream file(filename, std::ios::binary);

	if (!file.is_open())
	{
		addError(ErrorType::CANNOT_OPEN_FILE, filename, -1);
		return {};
	}

	file.seekg(0, std::ios::end);
	size_t size = file.tellg();

	if (file.fail())
	{
		addError(ErrorType::CANNOT_READ_FILE, "\nCannot determine file size: " + filename, -1);
		return {};
	}

	file.seekg(0, std::ios::beg);
	std::string content(size, '\0');
	file.read(&content[0], size);

	if (!file.good() && file.gcount() != static_cast<std::streamsize>(size))
	{
		addError(ErrorType::CANNOT_READ_FILE, filename, -1);
		return {};
	}

	qprintf(verbose, 0, "Successfully read file: %s (%i)", filename.c_str(), size);

	file.close();
	return content;
}


bool Assembler::writeFile(const std::vector<instruction_t>& instructions, std::string output_file, bool verilog_style)
{
	qprintf(verbose, 1, "%s\n%s", __func__, output_file.c_str());

	std::ofstream file;

	if (verilog_style)
	{
		file.open(output_file);
	}
	else
	{
		file.open(output_file, std::ios::binary);
	}

	if (!file.is_open())
	{
		if (verbose)
			std::cout << "Fail" << std::endl;

		addError(
			ErrorType::CANNOT_OPEN_FILE,
			output_file,
			-1
		);
		return false;
	}

	if (verbose)
		std::cout << "Ok" << std::endl;

	if (verilog_style)
	{
		if (verbose)
			std::cout << "Writing Verilog format (" << instructions.size() << " instructions)... ";

		for (const auto& instruction : instructions)
		{
			file << instructionToBinaryString(instruction) << std::endl;
		}
	}
	else
	{
		if (verbose)
			std::cout << "Writing binary format (" << instructions.size() * sizeof(instruction_t) << " bytes)... ";

		file.write(reinterpret_cast<const char*>(instructions.data()),
			instructions.size() * sizeof(instruction_t));
	}

	if (!file.good())
	{
		if (verbose)
			std::cout << "Fail" << std::endl;

		addError(
			ErrorType::CANNOT_WRITE_FILE,
			output_file,
			-1
		);
		return false;
	}

	if (verbose)
		std::cout << "Ok" << std::endl;

	file.close();
	return true;
}

std::vector<instruction_t> Assembler::assemble_blocks()
{
	std::vector<instruction_t> result(total_size, 0);

	for (const auto& block : blocks)
	{
		if (block.assembled_instructions.empty())
			continue;

		int start_pos = block.base_address;

		if (start_pos + block.size > total_size)
		{
			std::stringstream ss;
			ss << "Block '" << block.label << "' exceeds total size:" << std::endl
				<< "  Position: 0x" << std::hex << start_pos << std::endl
				<< "  Size: 0x" << block.size << std::endl
				<< "  Total size: 0x" << total_size;

			addError(ErrorType::ASSEMBLE_INTERNAL_ERROR, ss.str(), -1, true);
			return {};
		}

		std::copy(
			block.assembled_instructions.begin(),
			block.assembled_instructions.end(),
			result.begin() + start_pos
		);	
	}

	return result;
}

std::vector<instruction_t> Assembler::assemble(std::string source_code, int rom_size, bool verbose)
{
	clear();
	this->verbose = verbose;
	ROM_SIZE = rom_size;

	qprintf(verbose, 1, __func__);

	const std::list<std::string> lines = split_text_to_lines(source_code, true);

	if (!first_pass(lines))
		return {};

	if (!second_pass(lines))
		return {};

	return assemble_blocks();
}


