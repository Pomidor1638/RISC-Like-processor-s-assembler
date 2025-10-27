#pragma once

#include "common.h"

// non-assembler utils

std::string trim(const std::string& s);
std::string delete_comments(const std::string& s);

std::list<std::string> parse_instruction(const std::string& line);

std::list<std::string> parse_directiveData(const std::string& line);
std::list<std::string> parse_directiveString(const std::string& line);
std::list<std::string> parse_directiveLoadFile(const std::string& line);

std::vector<std::string> splitLines(const std::string& str);
std::list<std::string> split_text_to_lines(const std::string& text, bool trim_lines = false);

// assembler-only utils


bool is_intersect(int x, int x_size, int y, int y_size);

instruction_t packInstruction(int opcode, int rd, int rs, int rt);


bool isLabel(std::string token, bool is_arg);
bool isDirective(const std::string& line);
bool isMacro(const std::string& token);
bool isOpcode(const std::string& token, INSTRUCTION_META& meta);
bool isRegister(const std::string& token, int& regnum);
bool isValue(const std::string& token, int& value);
bool isInstruction(std::string line);

bool isValidInstruction(std::string line);
bool isValidIdentifier(const std::string& name);
bool isEntryPoint(const std::string& token, bool check_label = true);

std::string instructionToBinaryString(instruction_t instruction);
int getOpcodeSize(const std::string& opcode);



void qprintf(bool verbose, int level, const char* format, ...);
std::string qsprintf(const char* format, ...);