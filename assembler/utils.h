#pragma once

#include "common.h"


// non-assembler utils

std::string trim(const std::string& s);
std::list<std::string> parse_line(const std::string& line);
std::list<std::string> split_text_to_lines(const std::string& text, bool trim_lines = false);

// assembler-only utils

instruction_t packInstruction(int opcode, int func, int rd = 0, int rs = 0, int rt = 0);


bool isLabel(const std::string& token);
bool isOpcode(const std::string& token, OPCODE_META& meta);
bool isRegister(const std::string& token, int& regnum);
bool isImmidiate(const std::string& token, int& value);

bool isValidIdentifier(const std::string& name);

bool isEntryPoint(const std::string& token, bool check_label = true);