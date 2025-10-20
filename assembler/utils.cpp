
#include "utils.h"


std::string trim(const std::string& str) 
{
    auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
    auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();

    return (start < end) ? std::string(start, end) : "";
}

std::list<std::string> parse_line(const std::string& line)
{
    std::list<std::string> tokens;

    // Delete all after // (comments)

    std::string clean_line = line;
    size_t comment_pos = clean_line.find("//");
    if (comment_pos != std::string::npos) {
        clean_line = clean_line.substr(0, comment_pos);
    }

    std::stringstream ss(clean_line);
    std::string token;

    while (std::getline(ss, token, ','))
    {
        token = trim(token);

        if (!token.empty())
            tokens.push_back(token);

    }

    return tokens;
}

std::list<std::string> split_text_to_lines(const std::string& text, bool trim_lines)
{
    std::list<std::string> lines;
    std::stringstream ss(text);
    std::string line;

    while (std::getline(ss, line)) 
    {

        if (trim_lines)
            line = trim(line);

        lines.push_back(line);
    }

    return lines;
}

bool isLabel(const std::string& token) {

    if (token.empty()) 
        return false;

    if (token.back() != ':') 
        return false;

    if (token.find('.') == 0) 
        return false;

    std::string name = token.substr(0, token.length() - 1);
    return isValidIdentifier(name);
}


bool isOpcode(const std::string& token, OPCODE_META& meta)
{
    auto it = OPCODES.find(token);

    if (it != OPCODES.end()) {
        meta = it->second;
        return true;
    }

    meta = {};
    return false;
}

bool isRegister(const std::string& token, int& regnum)
{
    return REGISTERS.find(token) != REGISTERS.end();
}

bool isImmediate(const std::string& token, int& value)
{
    if (token.empty()) 
        return false;

    try {
        size_t pos = 0;
        value = std::stoi(token, &pos, 0);

        return pos == token.length() && value >= 0 && value <= 0xFFFF;
    }
    catch (...) {
        return false;
    }
}

bool isValidIdentifier(const std::string& name) 
{
    if (name.empty()) 
        return false;

    if (!std::isalpha(name[0]) && name[0] != '_') 
        return false;
    

    for (char c : name)
    {
        if (!std::isalnum(c) && c != '_') 
            return false;
    }

    return true;
}

bool isEntryPoint(const std::string& token, bool check_label) 
{
    if (!check_label) 
    {
        if (!isLabel(token))
            return false;
    }
    
    std::string name = token.substr(0, token.length() - 1);

    return (name == ENTRY_POINT);
}


instruction_t packInstruction(int opcode, int func, int rd, int rs, int rt)
{
    static const instruction_t OPCODE_MASK = (1 << OPCODE_SIZE) - 1;
    static const instruction_t REG_MASK = (1 << REG_ADDRESS_SIZE) - 1;
    static const instruction_t FUNC_MASK = (1 << FUNC_SIZE) - 1;

    // for additional safety
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