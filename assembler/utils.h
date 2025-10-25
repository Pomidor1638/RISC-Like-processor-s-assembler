#pragma once

#include "common.h"

// non-assembler utils

std::string trim(const std::string& s);
std::string delete_comments(const std::string& s);

std::list<std::string> parse_instruction(const std::string& line);
std::list<std::string> parse_directive(const std::string& line); // not preproccess

std::list<std::string> split_text_to_lines(const std::string& text, bool trim_lines = false);

// assembler-only utils


bool is_intersect(int x, int x_size, int y, int y_size);

instruction_t packInstruction(int opcode, int func = FUNC_NOP, int rd, int rs, int rt);


bool isLabel(const std::string& token);
bool isDirective(const std::string& token);
bool isMacro(const std::string& token);
bool isOpcode(const std::string& token, INSTRUCTION_META& meta);
bool isRegister(const std::string& token, int& regnum);
bool isValue(const std::string& token, int& value);
bool isInstruction(std::string line);

bool isValidInstruction(std::string line);
bool isValidIdentifier(const std::string& name);
bool isEntryPoint(const std::string& token, bool check_label = true);

int getOpcodeSize(const std::string& opcode);

std::string readFile(const std::string& filename, bool& error);