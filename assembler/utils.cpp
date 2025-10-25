
#include "utils.h"


std::string trim(const std::string& str)
{
    const std::string whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}


std::string delete_comments(const std::string& line)
{
    std::string clean_line = line;
    size_t comment_pos = clean_line.find("//");
    if (comment_pos != std::string::npos) {
        clean_line = clean_line.substr(0, comment_pos);
    }
    return clean_line;
}

std::list<std::string> parse_instruction(const std::string& line)
{
    std::list<std::string> tokens;

    std::string clean_line = line;

    std::stringstream ss(clean_line);
    std::string token;

    if (ss)
    {
        ss >> token;
        tokens.push_back(token);
    }

    while (std::getline(ss, token, ','))
    {
        token = trim(token);
        tokens.push_back(token);
    }

    return tokens;
}

std::list<std::string> parse_directive(const std::string& line)
{
    return {};
}

std::list<std::string> split_text_to_lines(const std::string& text, bool trim_lines)
{
    std::list<std::string> lines;
    std::stringstream ss(text);
    std::string line;

    while (std::getline(ss, line))
    {
        line = delete_comments(line);

        if (trim_lines) {
            line = trim(line);
        }

        if (!line.empty() || !trim_lines) {
            lines.push_back(line);
        }
    }
    return lines;
}


bool is_intersect(int x, int x_size, int y, int y_size)
{
    int x0 = x;
    int x1 = x + x_size - 1;

    int y0 = y;
    int y1 = y + y_size - 1;

    return (x0 <= y1) && (y0 <= x1);
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


bool isDirective(const std::string& token)
{
    return false;
}

bool isMacro(const std::string& token)
{
    return false;
}

bool isOpcode(const std::string& token, INSTRUCTION_META& meta)
{
    auto it = INSTRUCTIONS.find(token);

    if (it != INSTRUCTIONS.end()) {
        meta = it->second;
        return true;
    }

    meta = {};
    return false;
}

bool isRegister(const std::string& token, int& regnum)
{
    auto it = REGISTERS.find(token);

    regnum = 0;
    if (it == REGISTERS.end())
    {
        return false;
    }
    regnum = it->second;
    return true;
}

bool isValue(const std::string& token, int& value)
{
    if (token.empty())
        return false;

    try {
        size_t pos = 0;
        value = std::stoi(token, &pos, 0);

        return (pos == token.length()) && (value >= 0) && (value <= 0xFFFF);
    }
    catch (...) {
        return false;
    }
}

bool isInstruction(std::string line)
{
    std::stringstream ss(line);
    std::string opcode;

    ss >> opcode;

    INSTRUCTION_META meta;

    return isOpcode(opcode, meta);
}

bool isValidInstruction(std::string line)
{

    if (line[line.length() - 1] == ',')
        return false;

    auto tokens = parse_instruction(line);

    if (tokens.empty())
        return false;

    INSTRUCTION_META meta;

    std::string opcode = tokens.front();
    tokens.pop_front();

    if (!isOpcode(opcode, meta))
        return false;

    if (meta.opcode_args_num != tokens.size())
    {
        return false;
    }

    for (int i = 0; i < meta.opcode_args_num; i++)
    {
        std::string arg = tokens.front();
        tokens.pop_front();

        if (opcode == "LWI" && i == 1)
        {
            int value;
            if (
                !
                (
                    isValue(arg, value) ||
                    (isLabel(arg) + ':')
                    )
                )
                return false;
        }
        else
        {
            int regnum;
            if (!isRegister(arg, regnum))
            {
                return false;
            }
        }

    }

    return true;
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

    return (token == ENTRY_POINT);
}

std::string instructionToBinaryString(instruction_t instruction)
{
    constexpr size_t BITS_COUNT = sizeof(instruction_t) * 8;
    char buffer[BITS_COUNT + 1];

    for (int i = BITS_COUNT - 1; i >= 0; --i)
    {
        buffer[BITS_COUNT - 1 - i] = (instruction & (1ULL << i)) ? '1' : '0';
    }
    buffer[BITS_COUNT] = '\0';

    return std::string(buffer);
}


int getOpcodeSize(const std::string& opcode)
{
    return opcode == "LWI" ? 2 : 1;
}

instruction_t packInstruction(int opcode, int func, int rd, int rs, int rt)
{
    static const instruction_t OPCODE_MASK = (1 << OPCODE_SIZE) - 1;
    static const instruction_t    REG_MASK = (1 << REG_ADDRESS_SIZE) - 1;
    static const instruction_t   FUNC_MASK = (1 << FUNC_SIZE) - 1;

    // for additional safety
    opcode = opcode & OPCODE_MASK;
        rs =     rs &    REG_MASK;
        rt =     rt &    REG_MASK;
        rd =     rd &    REG_MASK;
      func =   func &   FUNC_MASK;

    instruction_t inst =
        (opcode << OPCODE_SHIFT) |
        (    rd <<     RD_SHIFT) |
        (    rs <<     RS_SHIFT) |
        (    rt <<     RT_SHIFT) |
        (  func <<   FUNC_SHIFT);

    return inst;
}