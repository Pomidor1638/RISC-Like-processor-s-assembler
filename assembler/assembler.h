#pragma once
#include "common.h"

class Assembler
{
public:

	bool assemble(std::string source_code, bool verbose);

	void writeFile(std::string output_file);

	std::string getError() const;

	bool is_ok();

	void clear();

protected:

	enum class ErrorType
	{
		UNKNWN_OPCODE = 1,
		UNKNWN_REGISTER,
		UNKNWN_DIRECTIVE,
		UNKNWN_MACRO,

		UNKNWN_TOKEN
	};

	struct Error
	{
		ErrorType type;
		std::string contents;
		int line;
	};

	std::map<std::string, address_t> labels;
	std::map<std::string, address_t> pointers;

	std::list<Error> errors;

	std::vector<instruction_t> assembled_instructions;

public:

	Assembler();

	virtual ~Assembler() = default;

};