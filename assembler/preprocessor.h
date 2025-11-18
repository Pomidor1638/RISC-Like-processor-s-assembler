#pragma once

#include "common.h"

/**
 * @class Preprocessor
 * @brief Assembler preprocessor for handling directives and macros
 *
 * Handles directives: #include, #define, #if, #ifdef, #ifndef,
 * #elif, #else, #endif, #macro, #endmacro
 */
class Preprocessor
{
public:
    // ==================== CONSTRUCTORS AND DESTRUCTOR ====================
    Preprocessor();
    virtual ~Preprocessor() = default;

    // ==================== PUBLIC INTERFACE ====================
    std::string preprocess(const std::string& source, bool verbose = false);
    bool is_ok() const;
    void clear();

private:
    // ==================== DATA STRUCTURES ====================

    /// @brief Preprocessor state machine states
    enum PREPROCESS_STATE
    {
        STATE_FETCHING = 0,                   ///< Normal line reading and processing
        STATE_READING_MACRO,                  ///< Reading macro body between #macro and #endmacro
        STATE_SKIPPING_CONDITION_BLOCK,       ///< Skipping block due to false condition
        STATE_READING_CONDITION_BLOCK,        ///< Reading block with true condition
        STATE_WAITING_NEXT_CONDITION_BLOCK,   ///< Waiting for #elif/#else after true condition
    };

    /// @brief Constant definition from #define directive
    struct Define
    {
        int start_line;        ///< Line number where definition starts
        std::string name;      ///< Constant name
        std::string value;     ///< Constant value
    };

    /// @brief Macro definition block
    struct MacroBlock
    {
        int start_line;                    ///< Line number where macro starts
        std::string name;                  ///< Macro name
        PreprocessorDirective type;        ///< Directive type
        std::list<std::string> args;       ///< Macro arguments
        std::list<std::string> lines;      ///< Macro body lines
    };

    /// @brief Preprocessor state frame for stack
    struct PreprocessorState
    {
        std::string name;                  ///< State identifier name
        PreprocessorDirective type;        ///< Directive type that created this state
        PREPROCESS_STATE state;            ///< Current processing state
        bool condition_met;                ///< Whether condition was met for conditional blocks
        size_t start_line;                 ///< Line number where state was pushed
        bool skip_content;                 ///< Whether to skip content in this state

        PreprocessorState(const std::string& n, PreprocessorDirective t,
            PREPROCESS_STATE s, size_t line)
            : name(n), type(t), state(s), start_line(line),
            condition_met(false), skip_content(false) {
        }
    };

    // ==================== CONSTANTS ====================
    static const int MAX_INCLUDE_DEPTH = 64;  ///< Maximum include nesting depth

private:
    // ==================== PROCESSING METHODS ====================
    bool prep_pass(const std::list<std::string>& lines);
    bool process_line(const std::string& line);

    // Preprocessor directive handlers
    bool preprocessInclude(const std::string& line);
    bool preprocessDefine(const std::string& line);
    bool preprocessElse(const std::string& line);
    bool preprocessElifdef(const std::string& line);
    bool preprocessElifndef(const std::string& line);
    bool preprocessIfdef(const std::string& line);
    bool preprocessIfndef(const std::string& line);
    bool preprocessEndif(const std::string& line);
    bool preprocessMacro(const std::string& line);
    bool preprocessEndMacro(const std::string& line);
    bool preprocessReadingDefinition(const std::string& line);

    bool expandMacroInvocation(const std::string& line) const;
    std::string expandMacros(const std::string& line) const;

    // ==================== STATE MANAGEMENT ====================
    bool pushState(const std::string& name, PreprocessorDirective type,
        PREPROCESS_STATE state, bool skip_content = false);
    bool popState();
    const PreprocessorState* getCurrentState() const;

    // State query methods
    bool shouldSkipCurrentBlock() const;
    bool isInConditionalBlock() const;
    bool isInMacroDefinition() const;

    // Condition management
    bool wasConditionMetInCurrentBlock() const;

    // Evaluation methods
    bool evaluateCondition(const std::string& condition) const;
    bool isDefined(const std::string& macroName) const;

    // ==================== MEMBER VARIABLES ====================
    bool is_okay;                           ///< Whether preprocessing completed successfully
    bool verbose;                           ///< Verbose output flag
    int line_num;                           ///< Current line number being processed
    int include_depth;                      ///< Current include nesting depth

    std::stack<PreprocessorState> state_stack;  ///< State stack for nested processing

    std::map<std::string, MacroBlock> blocks;   ///< Defined macro blocks
    std::map<std::string, Define> defines;      ///< Defined constants

    std::string preprocessed_code;          ///< Resulting preprocessed code
};
