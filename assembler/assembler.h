#pragma once
#include "common.h"

class Assembler
{
public:

	bool assemble(std::string source_code, bool verbose);

	bool writeFile(std::string output_file);

	std::string getError() const;

	bool is_ok();

	void clear();

protected:

	enum class ErrorType
	{
		UNEXCEPTED_OPCODE = 1,
		UNEXCEPTED_ARGS_NUM,
		UNEXCEPTED_REGISTER,
		UNEXCEPTED_DIRECTIVE,
		UNEXCEPTED_MACRO,
		UNEXCEPTED_TOKEN,

		CANNOT_OPEN_FILE,
		CANNOT_READ_FILE,
		CANNOT_WRITE_FILE,

		MULTIPLE_DEFENITIONS,
		MULTIPLE_ARGUMENTS,

		NO_ENTRY_POINT,

	};

	struct Error
	{
		ErrorType type;
		std::string contents;
		int line;
	};

	std::list<Error> errors;


	std::map<std::string, address_t> labels;
	std::map<std::string, address_t> pointers;

	struct AssembledBlock
	{
		std::string label;
		address_t base_addres;
		std::vector<instruction_t> assembled_intructions;
	};

	std::vector<AssembledBlock> assembled_blocks;

	AssembledBlock curBlock;


	bool has_entry_point;
	int line_num;

	void addError(ErrorType type, std::string contents, int line);

	void processInstruction(const std::string& line);
	void processLabel(const std::string& line);



public:

	Assembler();

	virtual ~Assembler() = default;

};