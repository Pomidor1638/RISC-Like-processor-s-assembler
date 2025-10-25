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
}

std::string Assembler::getError() const
{
	std::list<Error> crit_errs{};
	std::stringstream ss;
	for (const auto& x : errors)
	{
		if (x.critical)
		{
			crit_errs.push_back(x);
		}
		else
		{
			ss << "Error at line: " << x.line << getStrByErrorType(x.type) << x.contents << std::endl;
		}
	}


	for (const auto& x : crit_errs)
	{
		ss << CRITICAL_ERROR_HEADER;
		ss << "Error at line: " << x.line << getStrByErrorType(x.type) << x.contents << std::endl;
	}

	return ss.str();
}

void Assembler::addError(ErrorType type, std::string contents, int line, bool critical)
{
	Error err = { type, contents, -1 , critical };
	if (verbose)
	{
		if (critical)
			std::cerr << CRITICAL_ERROR_HEADER << std::endl;
		std::cerr << "Error at line: " << line << ' ' << getStrByErrorType(type) << contents << std::endl;
	}
	errors.push_back(err);
}

std::string Assembler::getStrByErrorType(ErrorType t) const
{
	static const std::map<ErrorType, std::string> error_messages = 
	{
		{ ErrorType::UNEXCEPTED_INSTRUCTION, "Unexpected opcode"			  },
		{ ErrorType::UNEXCEPTED_ARGS_NUM   , "Unexpected number of arguments" },
		{ ErrorType::UNEXCEPTED_REGISTER   , "Unexpected register"			  },
		{ ErrorType::UNEXCEPTED_DIRECTIVE  , "Unexpected directive"			  },
		{ ErrorType::UNEXCEPTED_MACRO      , "Unexpected macro"				  },
		{ ErrorType::UNEXCEPTED_TOKEN      , "Unexpected token"				  },
		{ ErrorType::CANNOT_OPEN_FILE      , "Cannot open file"				  },
		{ ErrorType::CANNOT_READ_FILE      , "Cannot read file"				  },
		{ ErrorType::CANNOT_WRITE_FILE	   , "Cannot write file"			  },
		{ ErrorType::MULTIPLE_DEFINITIONS  , "Multiple definitions"			  },
		{ ErrorType::MULTIPLE_ARGUMENTS	   , "Multiple arguments"			  },
		{ ErrorType::NO_ENTRY_POINT		   , "No entry point found"			  }
	};

	auto it = error_messages.find(t);
	if (it != error_messages.end()) {
		return it->second;
	}
	return "What the fuck is this ErrorType?";
}



bool Assembler::writeFile(std::string output_file)
{
	if (!is_ok())
	{
		return false;
	}

	if (verbose)
	{
		std::cout << __func__ << std::endl;
		std::cout << "Trying open ..." << output_file;
	}


	std::ofstream file(output_file, std::fstream::binary);

	if (!file.is_open())
	{

		if (verbose)
			std::cout << "Fails" << std::endl;

		addError(
			ErrorType::CANNOT_OPEN_FILE,
			output_file,
			-1
		);
		return false;
	}


	if (verbose)
		std::cout << "Ok" << std::endl;

	//const size_t size = assembled_instructions.size() * sizeof(instruction_t);
	//file.write((char*)assembled_instructions.data(), size);



	if (!file.good())
	{
		addError(
			ErrorType::CANNOT_WRITE_FILE,
			output_file,
			-1
		);
		return false;
	}

	return true;

}

std::vector<byte> Assembler::assemble_blocks()
{
	// Проверяем переполнение ROM
	if (total_size > ROM_SIZE)
	{
		addError(ErrorType::ROM_OVERFLOW, "", -1, true);
		return {};
	}

	// Проверяем наложение блоков (должна быть уже проверена в first_pass)
	// Но на всякий случай можно добавить быструю проверку
	for (auto it1 = blocks.begin(); it1 != blocks.end(); ++it1)
	{
		for (auto it2 = std::next(it1); it2 != blocks.end(); ++it2)
		{
			if (is_intersect(it1->base_address, it1->size, it2->base_address, it2->size))
			{
				std::stringstream ss;
				ss << "  " << it1->label << ": 0x" << std::hex << it1->base_address
					<< " - 0x" << it1->base_address + it1->size - 1 << std::endl
					<< "  " << it2->label << ": 0x" << std::hex << it2->base_address
					<< " - 0x" << it2->base_address + it2->size - 1;

				addError(ErrorType::ASSEMBLE_BLOCKS_OVERLAP, ss.str(), -1, true);
				return {};
			}
		}
	}

	std::vector<byte> result(total_size, 0x00);

	for (const auto& block : blocks)
	{
		if (block.assembled_instructions.empty())
			continue;

		int start_pos = block.base_address;

		// Проверяем, что блок помещается в результат
		if (start_pos + block.size > total_size)
		{
			std::stringstream ss;
			ss << "Block '" << block.label << "' exceeds total size:" << std::endl
				<< "  Position: 0x" << std::hex << start_pos << std::endl
				<< "  Size: 0x" << block.size << std::endl
				<< "  Total size: 0x" << total_size;

			addError(ErrorType::ASSEMBLE_BLOCKS_OVERLAP, ss.str(), -1, true);
			return {};
		}

		// Преобразуем инструкции в байты
		if constexpr (std::is_same_v<instruction_t, byte>) {
			std::copy(block.assembled_instructions.begin(),
				block.assembled_instructions.end(),
				result.begin() + start_pos);
		}
		else {
			for (size_t i = 0; i < block.assembled_instructions.size(); ++i) {
				instruction_t instr = block.assembled_instructions[i];
				const byte* bytes = reinterpret_cast<const byte*>(&instr);
				std::copy(bytes, bytes + sizeof(instruction_t),
					result.begin() + start_pos + i * sizeof(instruction_t));
			}
		}
	}

	return result;
}

std::vector<byte> Assembler::assemble(std::string source_code, int rom_size, bool v)
{
	clear();
	verbose = v;
	ROM_SIZE = rom_size;

	const std::list<std::string> lines = split_text_to_lines(source_code, true);

	if (!first_pass(lines))
		return {};

	if (!second_pass(lines))
		return {};

	return assemble_blocks();
}


