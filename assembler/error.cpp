#include "error.h"
#include "common.h"

ErrorLog error_log;

std::string ErrorLog::getErrors() const
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


void ErrorLog::addError(ErrorType type, std::string contents, int line, bool critical)
{
	Error err = { type, contents, line, critical };
	errors.push_back(err);
}

bool ErrorLog::has_errors() const
{
	return !errors.empty();
}

std::string ErrorLog::getStrByErrorType(ErrorType t) const
{
    static const std::map<ErrorType, std::string> error_messages =
    {
        { ASSEMBLER_UNEXCEPTED_INSTRUCTION, "Unexpected opcode" },
        { ASSEMBLER_UNEXCEPTED_OPCODE, "Unexpected opcode" },
        { ASSEMBLER_UNEXCEPTED_PLACEMENT, "Unexpected instruction placement" },
        { ASSEMBLER_UNEXCEPTED_LABEL, "Unexpected label" },
        { ASSEMBLER_UNEXCEPTED_ARGS_NUM, "Unexpected number of arguments" },
        { ASSEMBLER_UNEXCEPTED_REGISTER, "Unexpected register" },
        { ASSEMBLER_UNEXCEPTED_ARGUMENT, "Unexpected argument" },
        { ASSEMBLER_UNEXCEPTED_IMM_VALUE, "Unexpected immediate value" },
        { ASSEMBLER_UNEXCEPTED_DIRECTIVE, "Unexpected directive" },
        { ASSEMBLER_UNEXCEPTED_TOKEN, "Unexpected token" },

        { FILE_CANNOT_OPEN, "Cannot open file"                   },
        { FILE_CANNOT_READ, "Cannot read file"                   },
        { FILE_CANNOT_WRITE, "Cannot write file"                 },

        { ASSEMBLER_MULTIPLE_DEFINITIONS, "Multiple definitions" },
        { ASSEMBLER_MULTIPLE_UNEXCEPTED_ARGS, "Multiple unexcepted arguments" },

        { ASSEMBLER_BLOCKS_OVERLAP, "Blocks overlap" },
        { ASSEMBLER_INTERNAL_ERROR, "Internal assembler error" },

        { ASSEMBLER_ROM_OVERFLOW                   , "ROM overflow" },
        { ASSEMBLER_NO_ENTRY_POINT                 , "No entry point found" },

        // PREPROCESSOR ERROR MESSAGES
        { PREPROCESSOR_UNEXCEPTED_DIRECTIVE        , "Unexpected preprocessor directive"           },
        { PREPROCESSOR_MULTIPLE_DEFINITION         , "Multiple macro definitions"                  },
        { PREPROCESSOR_DEFINITION_WITHOUT_NAME     , "Macro definition without name"               },
        { PREPROCESSOR_UNEXCEPTED_MACRO            , "Unexpected macro"                            },
        { PREPROCESSOR_UNCLOSED_MACRO              , "Unclosed macro definition"                   },
        { PREPROCESSOR_DIRECTIVE_IN_MACRO_EXPANSION, "Preprocessor directive in macro expansion"   },
																								   
        { PREPROCESSOR_STACK_OVERFLOW              ,  "Preprocessor state stack overflow"          },
        { PREPROCESSOR_STACK_UNDERFLOW             ,  "Preprocessor state stack underflow"         },
        { PREPROCESSOR_INCLUDE_DEPTH_EXCEEDED      , "Include depth exceeded maximum limit"        },
        { PREPROCESSOR_UNCLOSED_BLOCK              , "Unclosed preprocessor block"                 },
        { PREPROCESSOR_INVALID_CONDITION           , "Invalid condition in preprocessor directive" },
        { PREPROCESSOR_CYCLIC_INCLUDE              , "Cyclic include detected"                     }
    };										       

    auto it = error_messages.find(t);
    if (it != error_messages.end())
    {
        return it->second;
    }
    return "What the fuck is this ErrorType?";
}


ErrorLog::ErrorLog()
	: errors{}
{

}


void ErrorLog::clear()
{
	errors.clear();
}