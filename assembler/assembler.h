#pragma once

#include "common.h"
#include "utils.h"

class Assembler
{
public:

	std::vector<instruction_t> assemble(std::string source_code, int rom_size, bool verbose = false);

	bool is_ok() const;
	void clear();

protected:

	bool is_okay;
	bool verbose;
	int  line_num;

	// assembler
	struct Block
	{
		// for first pass
		std::string label;
		int size;

		// for second pass
		int base_address;
		std::vector<instruction_t> assembled_instructions;
	};

	std::list<Block> blocks;
	std::map<std::string, Block*> block_by_label;

	Block* curBlock;
	bool has_entry_point;

	int ROM_SIZE;
	int total_size;

protected:

	void clearPreprocess();
	void clearAssembler();

	std::vector<instruction_t> assemble_blocks();

	// for first pass
	bool asm_first_pass(const std::list<std::string>& lines);

	bool analyzeInstruction(const std::string& line);
	bool analyzeLabel(const std::string& line);
	bool analyzeDirective(const std::string line);
	bool analyzeDirectiveString(const std::string line);
	bool analyzeDirectiveData(const std::string line);
	bool analyzeDirectiveLoadFile(const std::string line);

	// for second pass
	bool asm_second_pass(const std::list<std::string>& lines);

	bool processInstruction(const std::string& line);
	bool processNoArgsInstruction(const INSTRUCTION_META& instr);
	bool processOneArgInstruction(const INSTRUCTION_META& instr, const std::string& arg1);
	bool processTwoArgsInstruction(
		const INSTRUCTION_META& instr,
		const std::string& arg1,
		const std::string& arg2s
	);
	bool processThreeArgsInstruction(
		const INSTRUCTION_META& instr,
		const std::string& arg1,
		const std::string& arg2,
		const std::string& arg3
	);

	bool processLabel(const std::string& line);

public:

	Assembler();

	virtual ~Assembler() = default;

};