
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



std::list<std::string> parse_directiveData(const std::string& line)
{
    return {};
}
std::list<std::string> parse_directiveString(const std::string& line) 
{
    return {};
}
std::list<std::string> parse_directiveLoadFile(const std::string& line) 
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

        if (trim_lines) 
            line = trim(line);
        

        lines.push_back(line);
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

bool isLabel(std::string token, bool is_arg) {

    if (token.empty())
        return false;

    if (token.back() != ':' && !is_arg)
        return false;

    if (token.find('.') == 0)
        return false;

    if (!is_arg)
        token = token.substr(0, token.length() - 1);

    return isValidIdentifier(token);
}


bool isDirective(const std::string& line)
{

    std::stringstream ss(line);
    std::string directive;

    ss >> directive;

    return ASSEMBLER_DIRECTIVES.find(directive) != ASSEMBLER_DIRECTIVES.end();
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
            if (!(isValue(arg, value) || 
                isLabel(arg, true)))
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
        if (!isLabel(token, true))
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

std::vector<std::string> splitLines(const std::string& str) {
    std::vector<std::string> lines;
    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    return lines;
}

void qprintf(bool verbose, int level, const char* format, ...) {

    if (!verbose)
        return;

    va_list args;
    va_start(args, format);

    char buffer[1024 + 1];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    buffer[1024] = '\0';

    if (!level)
    {
        std::cout << buffer;
        return;
    }

    std::vector<std::string> lines = splitLines(buffer);

    std::string borderChar;
    int minWidth;
    switch (level) {
    case 1: 
        borderChar = "#"; 
        minWidth = 40; 
        break; 
    case 2: 
        borderChar = "*"; 
        minWidth = 36; 
        break;
    case 3: 
        borderChar = "-"; 
        minWidth = 32; 
        break;
    case 4: 
        borderChar = "~"; 
        minWidth = 28; 
        break;
    default: 
        borderChar = "-"; 
        minWidth = 32; 
        break;
    }

    size_t maxLineLength = 0;
    for (const auto& line : lines) {
        if (line.size() > maxLineLength) 
            maxLineLength = line.size();
    }
    int width = std::max(static_cast<int>(maxLineLength) + 6, minWidth);

    std::string borderLine(width, borderChar[0]);
    std::cout << '\r' << borderLine << std::endl;

    for (const auto& line : lines) {
        int padding = (width - static_cast<int>(line.size()) - 2) / 2;
        std::cout << borderChar
            << std::string(padding, ' ')
            << line
            << std::string(width - padding - line.size() - 2, ' ')
            << borderChar << std::endl;
    }

    std::cout << borderLine << std::endl;
}

std::string qsprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    int size = std::vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (size < 0) {
        va_end(args);
        return {};
    }

    std::vector<char> buffer(size + 1);
    std::vsnprintf(buffer.data(), buffer.size(), format, args);
    va_end(args);

    return std::string(buffer.data(), size);
}

instruction_t packInstruction(int opcode, int rd, int rs, int rt)
{
    static const instruction_t OPCODE_MASK = (1 << OPCODE_SIZE) - 1;
    static const instruction_t    REG_MASK = (1 << REG_ADDRESS_SIZE) - 1;

    // for additional safety
    opcode = opcode & OPCODE_MASK;
        rs =     rs &    REG_MASK;
        rt =     rt &    REG_MASK;
        rd =     rd &    REG_MASK;

    instruction_t inst =
        (opcode << OPCODE_SHIFT) |
        (    rd <<     RD_SHIFT) |
        (    rs <<     RS_SHIFT) |
        (    rt <<     RT_SHIFT);

    return inst;
}