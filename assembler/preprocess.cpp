#include "preprocessor.h"
#include "utils.h"

void Preprocessor::clear()
{
	is_okay = false;
	verbose = false;
	line_num = 0;
}

bool Preprocessor::is_ok() const
{
	return is_okay;
}


Preprocessor::Preprocessor()
	: is_okay(false)
	, verbose(false)
	, line_num(0)
{

}

bool Preprocessor::preprocessInclude(std::list<std::string>& tokens)
{
	include_depth++;
	return false;
}
bool Preprocessor::preprocessDefine(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessIf(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessElif(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessElse(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessElifdef(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessElifndef(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessIfdef(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessIfndef(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessEndif(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessMacro(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessEndMacro(std::list<std::string>& tokens)
{
	return false;
}
bool Preprocessor::preprocessReadingDefinition(const std::string& line)
{
	return false;
}

bool Preprocessor::prep_pass(const std::list<std::string>& lines)
{
	qprintf(verbose, 2, __func__);

	if (lines.empty())
		return false;

	for (const auto& line : lines)
	{
		if (line.empty())
			continue;

		if (isPreprocessDirective(line))
		{

			if (!state_stack.empty())
			{
				if (
					state_stack.top().state == STATE_READING_DEFINE ||
					state_stack.top().state == STATE_READING_MACRO
					)
				{
					error_log.addError(ErrorLog::PREPROCESSOR_DIRECTIVE_IN_MACRO_EXPANSION, line, line_num, true);
					return false;
				}
			}

			auto tokens = parse_preprocess_directive(line.substr(1));
			PreprocessorDirective type = PREPROCESSOR_DIRECTIVES.at(tokens.front());
			tokens.pop_front();

			switch (type)
			{
			case PREP_INCLUDE:
				preprocessInclude(tokens);
				break;
			case PREP_DEFINE:
				preprocessDefine(tokens);
				break;
			case PREP_IF:
				preprocessIf(tokens);
				break;
			case PREP_ELIF:
				preprocessElif(tokens);
				break;
			case PREP_ELSE:
				preprocessElse(tokens);
				break;
			case PREP_ELIFDEF:
				preprocessElifdef(tokens);
				break;
			case PREP_ELIFNDEF:
				preprocessElifndef(tokens);
				break;
			case PREP_IFDEF:
				preprocessIfdef(tokens);
				break;
			case PREP_IFNDEF:
				preprocessIfndef(tokens);
				break;
			case PREP_ENDIF:
				preprocessEndif(tokens);
				break;
			case PREP_MACRO:
				preprocessMacro(tokens);
				break;
			case PREP_ENDMACRO:
				preprocessEndMacro(tokens);
				break;
			default:
				error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
				break;
			}
		}
		else if (!state_stack.empty())
		{
			const auto& t = state_stack.top();
			switch (t.state)
			{
			case STATE_FETCHING:
				break;
			case STATE_READING_DEFINE:
			case STATE_READING_MACRO:
				preprocessReadingDefinition(line);
				break;
			case STATE_READING_CONDITION_BLOCK:
				if (!error_log.has_errors())
				{
					preprocessed_code += line + '\n';
				}
				break;
			case STATE_SKIPPING_CONDITION_BLOCK:
			case STATE_WAITING_NEXT_CONDITION_BLOCK:
				/*Just skip until face the end of condition or next one*/
			default:
				break;
			}

		}
	}

	return false;
}



std::string Preprocessor::preprocess(const std::string& source, bool verbose)
{
	clear();
	this->verbose = verbose;

	qprintf(verbose, 1, __func__);

	const std::list<std::string> lines = split_text_to_lines(source, true);

	if (!prep_pass(lines))
	{
		return {};
	}

	is_okay = true;

	return preprocessed_code;
}
