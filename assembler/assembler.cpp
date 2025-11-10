#include "assembler.h"
#include "utils.h"

Assembler::Assembler()
	: blocks{}
	, block_by_label{}
	, curBlock(nullptr)
	, has_entry_point(false)
	, line_num(0)
	, verbose(0)
	, ROM_SIZE(0)
	, total_size(0)
{

}


bool Assembler::is_ok() const
{
	return is_okay;
}

void Assembler::clear()
{
	verbose = false;
	line_num = 0;

	blocks.clear();
	block_by_label.clear();

	curBlock = nullptr;
	has_entry_point = false;

	ROM_SIZE = 0;
	total_size = 0;
	is_okay = true;
}

void Assembler::clearAssembler()
{
	blocks.clear();
	block_by_label.clear();

	curBlock = nullptr;
	has_entry_point = false;

	ROM_SIZE = 0;
	total_size = 0;
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

			error_log.addError(ErrorLog::ASSEMBLER_INTERNAL_ERROR, ss.str(), -1, true);
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

	if (!asm_first_pass(lines))
		return {};

	if (!asm_second_pass(lines))
		return {};

	return assemble_blocks();
}


