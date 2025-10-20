#pragma once

#include "common.h"


// non-assembler utils

std::string trim(const std::string& s);
std::list<std::string> parse_line(const std::string& line);
std::list<std::string> split_text_to_lines(const std::string& text);

// assembler-only utils

instruction_t packInstruction(int opcode, int func, int rd = 0, int rs = 0, int rt = 0);


