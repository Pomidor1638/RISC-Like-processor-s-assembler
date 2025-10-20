
#include "utils.h"


std::string trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
    auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();

    return (start < end) ? std::string(start, end) : "";
}

std::list<std::string> parse_line(const std::string& line)
{
    std::list<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, ',')) {

        token = trim(token);

        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

std::list<std::string> split_text_to_lines(const std::string& text)
{
    std::list<std::string> lines;
    std::stringstream ss(text);
    std::string line;

    while (std::getline(ss, line)) 
    {
        lines.push_back(line);
    }

    return lines;
}


instruction_t packInstruction(int opcode, int func, int rd = 0, int rs = 0, int rt = 0)
{
    static const instruction_t OPCODE_MASK = (1 << OPCODE_SIZE) - 1;
    static const instruction_t REG_MASK = (1 << REG_ADDRESS_SIZE) - 1;
    static const instruction_t FUNC_MASK = (1 << FUNC_SIZE) - 1;

    opcode = opcode & OPCODE_MASK; 
        rs =     rs &    REG_MASK;            
        rt =     rt &    REG_MASK;            
        rd =     rd &    REG_MASK;            
      func =   func &   FUNC_MASK;       

    instruction_t inst =
        (opcode << OPCODE_SHIFT) |
        (    rs <<     RS_SHIFT) |
        (    rt <<     RT_SHIFT) |
        (    rd <<     RD_SHIFT) |
        (  func <<   FUNC_SHIFT);

    return inst;
}