#pragma once
#include <string>
#include <list>

class ErrorLog
{
public:

	enum ErrorType;

	std::string getErrors() const;
	void addError(ErrorType type, std::string contents, int line, bool critical = false);
	std::string getStrByErrorType(ErrorType t) const;
	bool has_errors() const;

public:

	enum ErrorType
	{
		ASSEMBLER_UNEXCEPTED_INSTRUCTION = 1,
		ASSEMBLER_UNEXCEPTED_OPCODE,
		ASSEMBLER_UNEXCEPTED_INSTRUCTION_PLACEMENT,
		ASSEMBLER_UNEXCEPTED_LABEL,
		ASSEMBLER_UNEXCEPTED_ARGS_NUM,
		ASSEMBLER_UNEXCEPTED_REGISTER,
		ASSEMBLER_UNEXCEPTED_ARGUMENT,
		ASSEMBLER_UNEXCEPTED_IMM_VALUE,
		ASSEMBLER_UNEXCEPTED_DIRECTIVE,
		ASSEMBLER_UNEXCEPTED_TOKEN,
		 
		ASSEMBLER_MULTIPLE_DEFINITIONS,
		ASSEMBLER_MULTIPLE_UNEXCEPTED_ARGS,

		ASSEMBLER_BLOCKS_OVERLAP,
		ASSEMBLER_INTERNAL_ERROR,

		ASSEMBLER_ROM_OVERFLOW,
		ASSEMBLER_NO_ENTRY_POINT,

		PREPROCESSOR_UNEXCEPTED_DIRECTIVE,
		PREPROCESSOR_UNEXCEPTED_MACRO,
		PREPROCESSOR_UNCLOSED_MACRO,
		PREPROCESSOR_DIRECTIVE_IN_MACRO_EXPANSION,

		FILE_CANNOT_OPEN,
		FILE_CANNOT_READ,
		FILE_CANNOT_WRITE,

	};


private:

	struct Error
	{
		ErrorType type;
		std::string contents;
		int line;
		bool critical;
	};

	std::list<Error> errors;


};

extern ErrorLog error_log;