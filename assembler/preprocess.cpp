
#include "preprocessor.h"
#include "utils.h"


// ==================== ОСНОВНЫЕ МЕТОДЫ ====================

void Preprocessor::clear()
{
    is_okay = false;
    line_num = 0;
    include_depth = 0;

    while (!state_stack.empty()) state_stack.pop();
    blocks.clear();
    defines.clear();
    preprocessed_code.clear();
}

bool Preprocessor::is_ok() const
{
    return is_okay && state_stack.empty();
}

Preprocessor::Preprocessor()
    : is_okay(false)
    , verbose(false)
    , line_num(0)
    , include_depth(0)
{
}

// ==================== STACK HELPER METHODS ====================

bool Preprocessor::pushState(const std::string& name, PreprocessorDirective type,
    PREPROCESS_STATE state, bool skip_content)
{
    if (state_stack.size() > MAX_INCLUDE_DEPTH) {
        error_log.addError(ErrorLog::PREPROCESSOR_STACK_OVERFLOW, name, line_num);
        is_okay = false;
        return false;
    }

    PreprocessorState new_state(name, type, state, line_num);
    new_state.condition_met = !skip_content;
    new_state.skip_content = skip_content;

    state_stack.push(new_state);

    qprintf(verbose, 3, "PUSH STATE: %s (skip: %d, condition_met: %d)",
        name.c_str(), skip_content, !skip_content);

    return true;
}

bool Preprocessor::popState()
{
    if (state_stack.empty()) {
        error_log.addError(ErrorLog::PREPROCESSOR_STACK_UNDERFLOW, "", line_num);
        is_okay = false;
        return false;
    }

    auto& state = state_stack.top();
    qprintf(verbose, 3, "POP STATE: %s", state.name.c_str());

    state_stack.pop();
    return true;
}

const Preprocessor::PreprocessorState* Preprocessor::getCurrentState() const
{
    return state_stack.empty() ? nullptr : &state_stack.top();
}

bool Preprocessor::shouldSkipCurrentBlock() const
{
    if (state_stack.empty()) return false;
    const auto& current = state_stack.top();
    return current.skip_content ||
        current.state == STATE_SKIPPING_CONDITION_BLOCK ||
        current.state == STATE_WAITING_NEXT_CONDITION_BLOCK;
}

bool Preprocessor::isInConditionalBlock() const
{
    if (state_stack.empty()) return false;
    auto st = state_stack.top().state;
    return st == STATE_READING_CONDITION_BLOCK ||
        st == STATE_SKIPPING_CONDITION_BLOCK ||
        st == STATE_WAITING_NEXT_CONDITION_BLOCK;
}

bool Preprocessor::isInMacroDefinition() const
{
    return !state_stack.empty() && (state_stack.top().state == STATE_READING_MACRO);
}

bool Preprocessor::wasConditionMetInCurrentBlock() const
{
    return !state_stack.empty() && state_stack.top().condition_met;
}

bool Preprocessor::evaluateCondition(const std::string& condition) const
{
    if (condition.empty()) return false;

    try {
        int value;
        if (condition.find("0x") == 0 || condition.find("0X") == 0) {
            value = std::stoi(condition, nullptr, 16);
        }
        else {
            value = std::stoi(condition);
        }
        return (value != 0);
    }
    catch (const std::exception&) {
        return !condition.empty();
    }
}

bool Preprocessor::isDefined(const std::string& macroName) const
{
    bool found = blocks.find(macroName) != blocks.end() ||
        defines.find(macroName) != defines.end();

    qprintf(verbose, 4, "isDefined('%s') = %d", macroName.c_str(), found);
    return found;
}

// ==================== HELPERS: TRIM / SPLIT ARGS ====================

static std::string trim_copy(const std::string& s) { return trim(s); }

static std::vector<std::string> split_args_by_comma(const std::string& s)
{
    std::vector<std::string> out;
    std::string tmp;
    std::istringstream iss(s);
    // manual parse to preserve tokens with spaces trimmed
    size_t i = 0;
    std::string cur;
    while (i < s.size()) {
        if (s[i] == ',') {
            out.push_back(trim(cur));
            cur.clear();
            ++i;
            continue;
        }
        cur.push_back(s[i++]);
    }
    if (!cur.empty()) out.push_back(trim(cur));
    return out;
}

static std::list<std::string> split_tokens_space(const std::string& s)
{
    std::list<std::string> out;
    std::istringstream iss(s);
    std::string tok;
    while (iss >> tok) out.push_back(tok);
    return out;
}

// ==================== MACRO EXPANSION ====================

// expandMacros: разворачивает только #define (константы)
std::string Preprocessor::expandMacros(const std::string& line) const
{
    std::string result = line;

    // перебираем defines от конца (последние определённые имеют приоритет)
    for (auto it = defines.rbegin(); it != defines.rend(); ++it)
    {
        const auto& def = *it; // pair<const string, Define>
        const std::string& name = def.first;
        const std::string& value = def.second.value;

        size_t pos = 0;
        while ((pos = result.find(name, pos)) != std::string::npos)
        {
            bool is_whole_word = true;

            if (pos > 0 && (isalnum(result[pos - 1]) || result[pos - 1] == '_')) is_whole_word = false;
            size_t after = pos + name.length();
            if (after < result.length() && (isalnum(result[after]) || result[after] == '_')) is_whole_word = false;

            if (is_whole_word) {
                result.replace(pos, name.length(), value);
                pos += value.length();
            }
            else {
                pos += name.length();
            }
        }
    }

    return result;
}

// expandMacroInvocation: пытается распознать вызов макроса и заменить его телом.
// Метод объявлен const в заголовке — чтобы сохранить сигнатуру, здесь используется const_cast
// для модификации preprocessed_code. Если вы готовы поменять сигнатуру на неконстантную,
// рекомендую убрать const_cast.

bool Preprocessor::expandMacroInvocation(const std::string& line) const
{
    std::string trimmed = trim(line);
    if (trimmed.empty()) return false;

    // Получаем первое слово — потенциальное имя макроса
    std::istringstream iss(trimmed);
    std::string first;
    if (!(iss >> first)) return false;

    // Если нет такого макроблока — не обработать
    auto itBlock = blocks.find(first);
    if (itBlock == blocks.end()) return false;

    // Это вызов макроса. Получаем остальную часть строки (аргументы вызова)
    std::string rest;
    std::getline(iss, rest);
    rest = trim(rest);

    // Разобрать аргументы вызова: ожидаем разделение запятыми
    std::vector<std::string> call_args;
    if (!rest.empty()) {
        call_args = split_args_by_comma(rest);
    }
    else {
        call_args = {};
    }

    const MacroBlock& block = itBlock->second;
    size_t expected_args = block.args.size();

    if (call_args.size() != expected_args) {
        // Неправильное число аргументов — ошибка, но не фатальная
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_MACRO,
            "Macro invocation with wrong args: " + trimmed, line_num);
        // пометить, но всё же нельзя разворачивать некорректно; просто вернём false
        const_cast<Preprocessor*>(this)->is_okay = false;
        return false;
    }

    // Подготовка словаря аргName->value
    std::vector<std::string> param_names;
    for (const auto& n : block.args) param_names.push_back(n);

    std::map<std::string, std::string> param_map;
    for (size_t i = 0; i < param_names.size(); ++i) {
        param_map[param_names[i]] = call_args[i];
    }

    // Стек развёртываний для обнаружения рекурсии — thread_local чтобы не мешать внешней логике
    static thread_local std::vector<std::string> expansion_stack;

    // Если уже разворачиваем этот макрос — циклическая рекурсия
    if (std::find(expansion_stack.begin(), expansion_stack.end(), block.name) != expansion_stack.end()) {
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_MACRO,
            "Recursive macro expansion detected: " + block.name, line_num);
        const_cast<Preprocessor*>(this)->is_okay = false;
        return false;
    }

    // Помещаем в стек
    expansion_stack.push_back(block.name);

    // Для каждого строчного тела - выполняем замену параметров (whole-word) и рекурсивно разворачиваем вложенные макросы
    for (const auto& body_line : block.lines) {
        std::string substituted = body_line;

        // Выполняем замену параметров — whole-word, безопасно
        for (const auto& p : param_map) {
            const std::string& pname = p.first;
            const std::string& pval = p.second;
            // регулярка с границами слова
            std::regex re("\\b" + std::regex_replace(pname, std::regex(R"([.^$|()\\[\]{}*+?])"), R"(\\$&)") + "\\b");
            substituted = std::regex_replace(substituted, re, pval);
        }

        // После подстановки разворачиваем defines
        substituted = expandMacros(substituted);

        // Попробовать вложенные макросы: если строка начинается с имени макроса, expandMacroInvocation раскроет его.
        // Чтобы avoid infinite loops, expandMacroInvocation использует тот же expansion_stack.
        bool nested_expanded = expandMacroInvocation(substituted);
        if (!nested_expanded) {
            // Добавляем строку в результат (const_cast используется из-за const сигнатуры)
            std::string* out = const_cast<std::string*>(&preprocessed_code);
            out->append(substituted);
            out->push_back('\n');
        }
    }

    // Убираем из стека
    expansion_stack.pop_back();

    return true;
}

// ==================== PREPROCESSOR DIRECTIVES ====================

bool Preprocessor::preprocessInclude(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    include_depth++;

    if (include_depth > MAX_INCLUDE_DEPTH) {
        error_log.addError(ErrorLog::PREPROCESSOR_INCLUDE_DEPTH_EXCEEDED, "", line_num);
        is_okay = false;
        include_depth--;
        return false;
    }

    std::string filename = parse_include(line);
    if (filename.empty()) {
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
        is_okay = false;
        include_depth--;
        return false;
    }

    std::string source_code;
    if (!readFile(filename, source_code, verbose)) {
        error_log.addError(ErrorLog::FILE_CANNOT_OPEN, filename, line_num);
        is_okay = false;
        include_depth--;
        return false;
    }

    // Сохраняем текущий line_num и используем локальный для включаемого файла,
    // чтобы номера строк ошибок внутри файла были корректными.
    int saved_line_num = line_num;
    line_num = 1;

    auto lines = split_text_to_lines(source_code, true);
    bool result = prep_pass(lines);

    // Восстанавливаем внешний line_num. Оставляем line_num равным saved_line_num:
    line_num = saved_line_num;

    include_depth--;
    if (!result) {
        is_okay = false;
    }
    return result;
}

bool Preprocessor::preprocessDefine(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    if (line.empty()) {
        error_log.addError(ErrorLog::PREPROCESSOR_DEFINITION_WITHOUT_NAME, line, line_num);
        is_okay = false;
        return false;
    }

    std::list<std::string> tokens = parse_define(line);
    if (tokens.empty()) {
        error_log.addError(ErrorLog::PREPROCESSOR_DEFINITION_WITHOUT_NAME, line, line_num);
        is_okay = false;
        return false;
    }

    std::string name = tokens.front();
    tokens.pop_front();

    if (!isValidIdentifier(name)) {
        error_log.addError(ErrorLog::PREPROCESSOR_DEFINITION_WITHOUT_NAME,
            "Invalid macro name: " + name, line_num);
        is_okay = false;
        return false;
    }

    if (blocks.find(name) != blocks.end() || defines.find(name) != defines.end()) {
        error_log.addError(ErrorLog::ASSEMBLER_MULTIPLE_DEFINITIONS, name, line_num);
        is_okay = false;
        return false;
    }

    Define def;
    def.start_line = line_num;
    def.name = name;

    if (!tokens.empty()) {
        std::ostringstream oss;
        for (auto it = tokens.begin(); it != tokens.end(); ++it) {
            if (it != tokens.begin()) oss << " ";
            oss << *it;
        }
        def.value = oss.str();
    }
    else {
        def.value = "";
    }

    defines[name] = def;
    qprintf(verbose, 2, "DEFINE: %s = %s", name.c_str(), def.value.c_str());

    return true;
}

bool Preprocessor::preprocessElse(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    if (state_stack.empty()) {
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
        is_okay = false;
        return false;
    }

    auto& current = state_stack.top();

    if (current.type != PREP_IFDEF && current.type != PREP_IFNDEF &&
        current.type != PREP_ELIFDEF && current.type != PREP_ELIFNDEF) {
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
        is_okay = false;
        return false;
    }

    if (current.condition_met) {
        current.skip_content = true;
        current.state = STATE_WAITING_NEXT_CONDITION_BLOCK;
    }
    else {
        current.skip_content = false;
        current.state = STATE_READING_CONDITION_BLOCK;
        current.condition_met = true;
    }

    return true;
}

bool Preprocessor::preprocessElifdef(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    if (state_stack.empty()) {
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
        is_okay = false;
        return false;
    }

    auto& current = state_stack.top();

    if (current.type != PREP_IFDEF && current.type != PREP_IFNDEF &&
        current.type != PREP_ELIFDEF && current.type != PREP_ELIFNDEF) {
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
        is_okay = false;
        return false;
    }

    if (current.condition_met) {
        current.skip_content = true;
        current.state = STATE_WAITING_NEXT_CONDITION_BLOCK;
    }
    else {
        std::string macroName = extract_macro_name(line);
        bool condition = isDefined(macroName);

        if (condition) {
            current.skip_content = false;
            current.state = STATE_READING_CONDITION_BLOCK;
            current.condition_met = true;
        }
        else {
            current.skip_content = true;
            current.state = STATE_WAITING_NEXT_CONDITION_BLOCK;
        }
    }

    return true;
}

bool Preprocessor::preprocessElifndef(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    if (state_stack.empty()) {
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
        is_okay = false;
        return false;
    }

    auto& current = state_stack.top();

    if (current.type != PREP_IFDEF && current.type != PREP_IFNDEF &&
        current.type != PREP_ELIFDEF && current.type != PREP_ELIFNDEF) {
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
        is_okay = false;
        return false;
    }

    if (current.condition_met) {
        current.skip_content = true;
        current.state = STATE_WAITING_NEXT_CONDITION_BLOCK;
    }
    else {
        std::string macroName = extract_macro_name(line);
        bool condition = !isDefined(macroName);

        if (condition) {
            current.skip_content = false;
            current.state = STATE_READING_CONDITION_BLOCK;
            current.condition_met = true;
        }
        else {
            current.skip_content = true;
            current.state = STATE_WAITING_NEXT_CONDITION_BLOCK;
        }
    }

    return true;
}

bool Preprocessor::preprocessIfdef(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    std::string macroName = extract_macro_name(line);
    if (macroName.empty() || !isValidIdentifier(macroName)) {
        error_log.addError(ErrorLog::PREPROCESSOR_DEFINITION_WITHOUT_NAME, line, line_num);
        is_okay = false;
        return false;
    }

    bool condition = isDefined(macroName);

    qprintf(verbose, 2, "IFDEF '%s': condition = %d", macroName.c_str(), condition);

    if (condition) {
        pushState("ifdef", PREP_IFDEF, STATE_READING_CONDITION_BLOCK, false);
    }
    else {
        pushState("ifdef", PREP_IFDEF, STATE_SKIPPING_CONDITION_BLOCK, true);
    }

    return true;
}

bool Preprocessor::preprocessIfndef(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    std::string macroName = extract_macro_name(line);
    if (macroName.empty() || !isValidIdentifier(macroName)) {
        error_log.addError(ErrorLog::PREPROCESSOR_DEFINITION_WITHOUT_NAME, line, line_num);
        is_okay = false;
        return false;
    }

    bool condition = !isDefined(macroName);

    qprintf(verbose, 2, "IFNDEF '%s': condition = %d", macroName.c_str(), condition);

    if (condition) {
        pushState("ifndef", PREP_IFNDEF, STATE_READING_CONDITION_BLOCK, false);
    }
    else {
        pushState("ifndef", PREP_IFNDEF, STATE_SKIPPING_CONDITION_BLOCK, true);
    }

    return true;
}

bool Preprocessor::preprocessEndif(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    if (state_stack.empty()) {
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
        is_okay = false;
        return false;
    }

    return popState();
}

bool Preprocessor::preprocessMacro(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    // Используем специальный парсер для макросов
    auto tokens = parse_macro_definition(line.substr(1)); // убираем #
    if (tokens.empty()) {
        error_log.addError(ErrorLog::PREPROCESSOR_DEFINITION_WITHOUT_NAME, line, line_num);
        is_okay = false;
        return false;
    }

    // tokens: ["macro", "NAME", "arg1", "arg2", ...] - теперь правильно разделено
    if (tokens.size() < 2) {
        error_log.addError(ErrorLog::PREPROCESSOR_DEFINITION_WITHOUT_NAME, line, line_num);
        is_okay = false;
        return false;
    }

    std::string macroName = tokens.front(); // "macro"
    tokens.pop_front();

    if (tokens.empty()) {
        error_log.addError(ErrorLog::PREPROCESSOR_DEFINITION_WITHOUT_NAME, line, line_num);
        is_okay = false;
        return false;
    }

    std::string name = tokens.front();
    tokens.pop_front();

    if (name.empty() || !isValidIdentifier(name)) {
        error_log.addError(ErrorLog::PREPROCESSOR_DEFINITION_WITHOUT_NAME, line, line_num);
        is_okay = false;
        return false;
    }

    if (blocks.find(name) != blocks.end() || defines.find(name) != defines.end()) {
        error_log.addError(ErrorLog::ASSEMBLER_MULTIPLE_DEFINITIONS, name, line_num);
        is_okay = false;
        return false;
    }

    // Create macro block and parse argument names
    MacroBlock block;
    block.start_line = line_num;
    block.name = name;
    block.type = PREP_MACRO;

    // Теперь аргументы правильно разделены
    for (const auto& a : tokens) {
        if (!isValidIdentifier(a)) {
            error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_MACRO,
                "Invalid macro argument: '" + a + "'", line_num);
            is_okay = false;
        }
        block.args.push_back(a);
    }

    blocks[name] = block;

    // Enter macro reading state
    return pushState(name, PREP_MACRO, STATE_READING_MACRO, false);
}

bool Preprocessor::preprocessEndMacro(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    if (state_stack.empty() || state_stack.top().type != PREP_MACRO) {
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
        is_okay = false;
        return false;
    }

    return popState();
}

bool Preprocessor::preprocessReadingDefinition(const std::string& line)
{
    qprintf(verbose, 3, "%s\n%s", __func__, line.c_str());

    if (state_stack.empty()) {
        return false;
    }

    auto& current = state_stack.top();
    std::string macroName = current.name;

    // Check if this is #endmacro directive
    if (isPreprocessDirective(line)) {
        std::string directive = get_preprocessor_directive(line);
        if (directive == "endmacro") {
            qprintf(verbose, 3, "Found #endmacro for macro '%s' - ending macro definition", macroName.c_str());
            // Pop state — энд макро закончился
            return popState();
        }
    }

    // Regular line - add to macro body
    auto it = blocks.find(macroName);
    if (it != blocks.end()) {
        it->second.lines.push_back(line);
    }
    else {
        // Не найден блок — ошибка, но продолжаем
        error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_MACRO, "Unknown macro while reading body: " + macroName, line_num);
        is_okay = false;
    }

    return true;
}

// ==================== MAIN PROCESSING LOGIC ====================

bool Preprocessor::process_line(const std::string& line)
{
    bool success = true;

    if (isPreprocessDirective(line))
    {
        if (isInMacroDefinition())
        {
            // Внутри определения макроса — только #endmacro обрабатывается как директива,
            // остальные директивы считаем текстом тела макроса.
            std::string directive = get_preprocessor_directive(line);
            if (directive == "endmacro") {
                preprocessReadingDefinition(line);
            }
            else {
                preprocessReadingDefinition(line);
            }
            line_num++;
            return true; // продолжаем
        }

        if (shouldSkipCurrentBlock()) {
            std::string directive = get_preprocessor_directive(line);
            if (directive != "else" && directive != "endif" &&
                directive != "elifdef" && directive != "elifndef") {
                line_num++;
                return true;
            }
        }

        auto tokens = parse_preprocess_directive(line.substr(1));
        if (tokens.empty()) {
            error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
            is_okay = false;
            line_num++;
            return false;
        }

        std::string directive_name = tokens.front();
        auto it = PREPROCESSOR_DIRECTIVES.find(directive_name);
        if (it == PREPROCESSOR_DIRECTIVES.end()) {
            error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
            is_okay = false;
            line_num++;
            return false;
        }

        PreprocessorDirective type = it->second;
        tokens.pop_front();

        switch (type)
        {
        case PREP_INCLUDE:
            success = preprocessInclude(line);
            break;
        case PREP_DEFINE:
            success = preprocessDefine(line);
            break;
        case PREP_ELSE:
            success = preprocessElse(line);
            break;
        case PREP_ELIFDEF:
            success = preprocessElifdef(line);
            break;
        case PREP_ELIFNDEF:
            success = preprocessElifndef(line);
            break;
        case PREP_IFDEF:
            success = preprocessIfdef(line);
            break;
        case PREP_IFNDEF:
            success = preprocessIfndef(line);
            break;
        case PREP_ENDIF:
            success = preprocessEndif(line);
            break;
        case PREP_MACRO:
            success = preprocessMacro(line);
            break;
        case PREP_ENDMACRO:
            // Не должно происходить когда в макро, но обработаем
            success = preprocessEndMacro(line);
            break;
        default:
            error_log.addError(ErrorLog::PREPROCESSOR_UNEXCEPTED_DIRECTIVE, line, line_num);
            is_okay = false;
            success = false;
            break;
        }

        if (!success) is_okay = false;
        line_num++;
        return success;
    }

    else if (!state_stack.empty())
    {
        const auto& t = state_stack.top();
        switch (t.state)
        {
        case STATE_FETCHING:
            // Сначала пытаемся развёрнуть вызов макроса (one-pass)
            if (!expandMacroInvocation(line)) {
                preprocessed_code += expandMacros(line) + '\n';
            }
            break;
        case STATE_READING_MACRO:
            preprocessReadingDefinition(line);
            break;
        case STATE_READING_CONDITION_BLOCK:
            if (!t.skip_content)
            {
                if (!expandMacroInvocation(line)) {
                    preprocessed_code += expandMacros(line) + '\n';
                }
            }
            break;
        case STATE_SKIPPING_CONDITION_BLOCK:
        case STATE_WAITING_NEXT_CONDITION_BLOCK:
            // пропускаем
            break;
        default:
            break;
        }
    }
    else
    {
        // No active states - expand macros
        if (!expandMacroInvocation(line)) {
            preprocessed_code += expandMacros(line) + '\n';
        }
    }

    line_num++;
    return true;
}

bool Preprocessor::prep_pass(const std::list<std::string>& lines)
{
    qprintf(verbose, 2, "%s", __func__);

    if (lines.empty()) return false;

    bool overall_ok = true;
    line_num = 1;
    for (const auto& line : lines)
    {
        if (line.empty()) {
            line_num++;
            continue;
        }

        if (!process_line(line))
            overall_ok = false;
        // не прерываем проход — собираем ошибки; is_okay помечается внутри при ошибках
    }

    return overall_ok;
}

std::string Preprocessor::preprocess(const std::string& source, bool verbose)
{
    qprintf(verbose, 1, "%s", __func__);

    clear();
    this->verbose = verbose;

    const std::list<std::string> lines = split_text_to_lines(source, true);

    if (!prep_pass(lines))
    {
        qprintf(verbose, 0, "PREPROCESS FAILED - errors: %d", error_log.has_errors());
        return {};
    }

    if (!state_stack.empty())
    {
        error_log.addError(ErrorLog::PREPROCESSOR_UNCLOSED_BLOCK, state_stack.top().name, line_num);
        qprintf(verbose, 0, "UNCLOSED BLOCK - errors: %d", error_log.has_errors());
        is_okay = false;
        return {};
    }

    is_okay = !error_log.has_errors();
    
    qprintf(verbose, 1, "PREPROCESS END - errors: %d\n", error_log.has_errors());

    if (!is_okay) 
        return {};

    return preprocessed_code;
}
