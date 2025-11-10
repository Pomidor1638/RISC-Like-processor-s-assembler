#include "common.h"

class Preprocessor
{
public:
	std::string preprocess(const std::string& source, bool verbose = false);

	bool is_ok() const;
	void clear();

protected:

	bool prep_pass(const std::list<std::string>& lines);


	bool preprocessInclude(std::list<std::string>& tokens);
	bool preprocessDefine(std::list<std::string>& tokens);
	bool preprocessIf(std::list<std::string>& tokens);
	bool preprocessElif(std::list<std::string>& tokens);
	bool preprocessElse(std::list<std::string>& tokens);
	bool preprocessElifdef(std::list<std::string>& tokens);
	bool preprocessElifndef(std::list<std::string>& tokens);
	bool preprocessIfdef(std::list<std::string>& tokens);
	bool preprocessIfndef(std::list<std::string>& tokens);
	bool preprocessEndif(std::list<std::string>& tokens);
	bool preprocessMacro(std::list<std::string>& tokens);
	bool preprocessEndMacro(std::list<std::string>& tokens);


	bool preprocessReadingDefinition(const std::string& line);

protected:

	enum PREPROCESS_STATE
	{
		STATE_FETCHING = 0,
		STATE_READING_DEFINE,
		STATE_READING_MACRO,
		STATE_SKIPPING_CONDITION_BLOCK,
		STATE_READING_CONDITION_BLOCK,
		STATE_WAITING_NEXT_CONDITION_BLOCK,
	};

	struct MacroBlock
	{
		int start_line;
		std::string				name;
		PreprocessorDirective   type;
		std::list<std::string>  args;
		std::list<std::string> lines;
	};

	struct MacroState
	{
		std::string name;
		MacroBlock* block;
		PREPROCESS_STATE state;
	};

	static const int MAX_INCLUDE_DEPTH = 64;

protected:

	bool is_okay;
	bool verbose;
	int  line_num;
	int include_depth;

	std::stack<MacroState> state_stack;
	std::map<std::string, MacroBlock> blocks;

	std::string preprocessed_code;

public:
	Preprocessor();
	virtual ~Preprocessor() = default;
};