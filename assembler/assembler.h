#pragma once

#include "common.h"
#include "utils.h"

class Assembler
{
public:

	bool assemble(std::string source_code, bool v);

	bool writeFile(std::string output_file);

	std::string getError() const;

	bool is_ok();
	void clear();

protected:

	bool verbose;

	enum class ErrorType
	{
		UNEXCEPTED_OPCODE = 1,
		UNEXCEPTED_OPCODE_PLACEMENT,
		UNEXCEPTED_ARGS_NUM,
		UNEXCEPTED_REGISTER,
		UNEXCEPTED_DIRECTIVE,
		UNEXCEPTED_MACRO,
		UNEXCEPTED_TOKEN,

		CANNOT_OPEN_FILE,
		CANNOT_READ_FILE,
		CANNOT_WRITE_FILE,

		MULTIPLE_DEFINITIONS,
		MULTIPLE_ARGUMENTS,

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

	struct AssembledBlock
	{
		// for first pass
		std::string label;
		int size;

		// for second pass
		int base_address;
		std::vector<instruction_t> assembled_instructions;
	};

	std::list<AssembledBlock> blocks;
	std::map<std::string, AssembledBlock*> block_by_label;

	AssembledBlock* curBlock;

	bool has_entry_point;
	int  line_num;

	// TODO: Refactor this error handling mess
	// Current state: spaghetti code that even I don't understand
	// Planned improvements:
	// - Separate error collection from formatting
	// - Use proper error categories
	// - Add error codes and suggestions
	// - Make it actually maintainable

	void addError(ErrorType type, std::string contents, int line, bool critical = false);
	std::string getStrByErrorType(ErrorType t) const;

	// for first pass

	bool first_pass(const std::list<std::string>& lines);

	bool analyzeInstruction(const std::string& line);
	bool analyzeLabel(const std::string& line);
	bool analyzeDirective(const std::string line);

	// for second pass
	bool second_pass(const std::list<std::string>& lines);

	void processInstruction(const std::string& line);
	//void processLabel(const std::string& line);
	//void processDirective(const std::string line); // Directives don't need to process

public:

	Assembler();

	virtual ~Assembler() = default;

};