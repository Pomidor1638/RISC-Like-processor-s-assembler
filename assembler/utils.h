#pragma once

#include "common.h"

// ============================================================================
// TEXT PROCESSING AND UTILITY FUNCTIONS
// ============================================================================

std::string trim(const std::string& s);
std::string delete_comments(const std::string& s);
std::vector<std::string> splitLines(const std::string& str);
std::list<std::string> split_text_to_lines(const std::string& text, bool trim_lines = false);

// ============================================================================
// PARSING FUNCTIONS
// ============================================================================

std::list<std::string> parse_instruction(const std::string& line);
std::list<std::string> parse_preprocess_directive(const std::string& line);
std::list<std::string> parse_directiveData(const std::string& line);
std::list<std::string> parse_directiveString(const std::string& line);
std::list<std::string> parse_directiveLoadFile(const std::string& line);

// ============================================================================
// INSTRUCTION PROCESSING AND VALIDATION
// ============================================================================

bool is_intersect(int x, int x_size, int y, int y_size);
instruction_t packInstruction(int opcode, int rd, int rs, int rt);
std::string instructionToBinaryString(instruction_t instruction);
int getOpcodeSize(const std::string& opcode);

// ============================================================================
// TOKEN IDENTIFICATION AND VALIDATION
// ============================================================================

bool isDirective(const std::string& line);
bool isInstruction(const std::string& line);
bool isPreprocessDirective(const std::string& line);
bool isPreprocessMacros(const std::string& line);
bool isValidInstruction(const std::string& line);
bool isMacro(const std::string& token);
bool isOpcode(const std::string& token, INSTRUCTION_META& meta);
bool isRegister(const std::string& token, int& regnum);
bool isValue(const std::string& token, int& value);
bool isLabel(std::string token, bool is_arg);
bool isEntryPoint(const std::string& token, bool check_label = true);
bool isValidIdentifier(const std::string& token);

// ============================================================================
// DEBUGGING AND OUTPUT UTILITIES
// ============================================================================

void qprintf(bool verbose, int level, const char* format, ...);
std::string qsprintf(const char* format, ...);

bool writeFile(const std::vector<instruction_t>& data, std::string output_file, bool verbose, bool verilog_style = false);
std::string readFile(const std::string& filename, bool verbose);