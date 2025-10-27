#pragma once

#include "common.h"
#include "utils.h"

class Assembler
{
public:

	std::vector<instruction_t> assemble(std::string source_code, int rom_size, bool verbose = false);

	bool writeFile(const std::vector<instruction_t>& data, std::string output_file, bool verilog_style = false);
	std::string readFile(const std::string& filename);

	std::string getError() const;

	bool is_ok();
	void clear();

protected:

	bool verbose;

	enum class ErrorType
	{
		UNEXCEPTED_INSTRUCTION = 1,
		UNEXCEPTED_OPCODE,
		UNEXCEPTED_INSTRUCTION_PLACEMENT,
		UNEXCEPTED_LABEL,
		UNEXCEPTED_ARGS_NUM,
		UNEXCEPTED_REGISTER,
		UNEXCEPTED_ARGUMENT,
		UNEXCEPTED_IMM_VALUE,
		UNEXCEPTED_DIRECTIVE,
		UNEXCEPTED_MACRO,
		UNEXCEPTED_TOKEN,

		CANNOT_OPEN_FILE,
		CANNOT_READ_FILE,
		CANNOT_WRITE_FILE,

		MULTIPLE_DEFINITIONS,
		MULTIPLE_UNEXCEPTED_ARGS,

		ASSEMBLE_BLOCKS_OVERLAP,
		ASSEMBLE_INTERNAL_ERROR,

		ROM_OVERFLOW,
		NO_ENTRY_POINT,
	};

	struct Error
	{
		ErrorType type;
		std::string contents;
		int line;
		bool critical;
	};

	std::list<Error> errors;

	struct Block
	{
		// for first pass
		std::string label;
		int origin;
		int size;

		// for second pass
		int base_address;
		std::vector<instruction_t> assembled_instructions;
	};

	std::list<Block> blocks;
	std::map<std::string, Block*> block_by_label;

	Block* curBlock;

	bool has_entry_point;
	int  line_num;

	int ROM_SIZE;

	int total_size;
	std::vector<instruction_t> assemble_blocks();


	void addError(ErrorType type, std::string contents, int line, bool critical = false);
	std::string getStrByErrorType(ErrorType t) const;

	// for first pass

	bool first_pass(const std::list<std::string>& lines);

	bool analyzeInstruction(const std::string& line);
	bool analyzeLabel(const std::string& line);
	bool analyzeDirective(const std::string line);
		bool analyzeDirectiveString(const std::string line);
		bool analyzeDirectiveData(const std::string line);
		bool analyzeDirectiveLoadFile(const std::string line);

	// for second pass
	bool second_pass(const std::list<std::string>& lines);

	bool processInstruction(const std::string& line);
	bool processNoArgsInstruction(const INSTRUCTION_META& instr);
	bool processOneArgInstruction(const INSTRUCTION_META& instr, const std::string& arg1);
	bool processTwoArgsInstruction(
		const INSTRUCTION_META& instr,
		const std::string& arg1,
		const std::string& arg2
	);
	bool processThreeArgsInstruction(
		const INSTRUCTION_META& instr,
		const std::string& arg1,
		const std::string& arg2,
		const std::string& arg3
	);

	bool processLabel(const std::string& line);


	//void processDirective(const std::string line);

public:

	Assembler();

	virtual ~Assembler() = default;

};