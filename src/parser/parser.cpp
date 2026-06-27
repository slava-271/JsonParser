#include "parser.hpp"

void JsonParser::PrintVisitor::operator()(std::monostate) const {
    std::print("null");
}

void JsonParser::PrintVisitor::operator()(const bool val) const {
    std::print("{}", val ? "true" : "false");
}

void JsonParser::PrintVisitor::operator()(const double val) const {
    std::print("{}", val);
}

void JsonParser::PrintVisitor::operator()(const longlong val) const {
    std::print("{}", val);
}

void JsonParser::PrintVisitor::operator()(const ulonglong val) const {
    std::print("{}", val);
}

void JsonParser::PrintVisitor::operator()(const std::string_view& val) const {
    std::print("\"{}\"", val);
}

void JsonParser::PrintVisitor::operator()(const std::string& val) const {
    std::print("\"{}\"", val);
}

void JsonParser::PrintVisitor::operator()(const JsonArray& arr) {
    if (arr.empty()) {
        std::print("{}", "[]");
        return;
    }
    std::println("[");

    tab.push_back('\t');

    for (size_t i = 0; i < arr.size() - 1; ++i) {
        std::print("{}", tab);
        std::visit(*this, arr[i].data);
        std::println(",");
    }

    std::print("{}", tab);
    std::visit(*this, arr[arr.size() - 1].data);

    std::println();
    std::print("{}{}", tab, "]");
    tab.pop_back();
}

void JsonParser::PrintVisitor::operator()(const JsonObject& obj) {
    if (obj.empty()) {
        std::print("{}", "{}");
        return;
    }

    std::println("{{");
    tab.push_back('\t');

    for (size_t i = 0; i < obj.size() - 1; ++i) {
        std::print("{}", tab);
        std::print("\"{}\" : ", obj[i].str);
        std::visit(*this, obj[i].val.data);
        std::println(",");
    }


    std::print("{}", tab);
    std::print("\"{}\" : ", obj[obj.size() - 1].str);
    std::visit(*this, obj[obj.size() - 1].val.data);

    std::println();
    std::print("{}{}", tab, "}");
    tab.pop_back();
}


void JsonParser::operator()() {
    ast = ParseValue();
    return;
}

void JsonParser::JsonPrint() {
    PrintVisitor vis;
    std::visit(vis, ast.data);
}



bool AppendUtf8(std::string& str, const ulong num) {
    if (num <= 0x7f) {
        str.push_back(num);
    }
    else if (num <= 0X7FF) {
        str.push_back(0b110'00000 | (num >> 6));
        str.push_back(0b10'000000 | (0b00000'111111 & num));
    } else if (num < 0xd800 || num > 0xdfff) {
        str.push_back(0b1110'0000 | (num >> 12));
        str.push_back(0b10'000000 | (0b0000'0000'00'111111 & (num >> 6)));
        str.push_back(0b10'000000 | (0b0000'0000'00'111111 & num));
    } else {
        if (num >= 0xd800 && num <= 0xdbff)
            return true;
        throw std::runtime_error("low surrogate is not possible");
    }
    return false;
}

std::string FormString(const std::string_view& str) {
    std::string res;
    res.reserve(str.size());
    size_t start = 0;
    for(size_t i = 0; i < str.size();++i) {
        start = i;
        size_t slash = str.find('\\', i);
        if (slash == std::string_view::npos) {
            res.append(str.data() + start, str.size() - start);
            break;
        }
        else {
            if (slash > start)
                res.append(str.data() + start, slash - start);
        }
        i = ++slash;
        switch (str[i]) {
        case '\\': res.push_back('\\');  break;
        case '"': res.push_back('"'); break;
        case '/': res.push_back('/'); break;
        case 'b': res.push_back('\b'); break;
        case 'f': res.push_back('\f'); break;
        case 'n': res.push_back('\n'); break;
        case 't': res.push_back('\t'); break;
        case 'r': res.push_back('\r'); break;
        case 'u': {
            ++i;
            ulong hex1;
            std::from_chars(str.data() + i, str.data() + i + 4, hex1, 16);
            bool isUtf16 = AppendUtf8(res, hex1);
            i += 3;
            if (isUtf16) {
            if (i < str.size()  && str[i + 1] == '\\' && str[i + 2] == 'u') {
                i += 3;
                ulong hex2;
                std::from_chars(str.data() + i, str.data() + i + 4, hex2, 16);
                i += 3;
                if (! (hex2 >= 0xdc00 && hex2 <= 0xdfff))
                    throw std::runtime_error("need surrogate pair. Error in string:" + std::string(str));
                else {
                    uint codePoint = 0x10000 + ( ((hex1 - 0xd800) << 10 ) | (hex2 - 0xdc00));
                    char buff[4];
                    buff[0] = 0b11110'000 | (codePoint >> 18);
                    buff[1] = 0b10'000000 | (0b0000'0000'00'111111 & (codePoint >> 12));
                    buff[2] = 0b10'000000 | (0b0000'0000'00'111111 & (codePoint >> 6));
                    buff[3] = 0b10'000000 | (0b0000'0000'00'111111 & codePoint);
                    res.append(buff, 4);
                }
            } else
                throw std::runtime_error("needed surrogate pair. Error in string:" + std::string(str));
            }
            break;
        }
        default:
            throw std::runtime_error("wrong char after \\");
        }
    }
    return res;
}

JsonObject JsonParser::ParseObject() {
    JsonObject obj{};
    bool isComma = true, bannedComma = false;
    while (it->token != R_BRACE && !bannedComma) {
        std::string_view key;
        if (it->token == STRING) {
            key = it->str;
        } else if (it->token == ESCAPE_STRING) {
            escList.emplace_back(FormString(it->str));
            key = escList.back();
        } else {
            throw std::runtime_error("expected string");
        }
        key.remove_prefix(1);
        key.remove_suffix(1);
        ++it;

        if (it->token != COLON)
            throw std::runtime_error("expected colon");

        ++it;

        JsonValue res = ParseValue();
        obj.emplace_back(key, std::move(res));

        isComma = it->token == COMMA;
        it += isComma;
        bannedComma = (it->token == R_BRACE) && isComma;
    }

    if (it->token != R_BRACE)
        throw std::runtime_error("expected }");

    if (bannedComma)
        throw std::runtime_error("comma before } is banned");

    ++it;
    return obj;
}


JsonObject JsonParser::ProfileObject() {
    JsonObject obj{};
    bool isComma = true, bannedComma = false;
    while (it->token != R_BRACE && !bannedComma) {
        std::string_view key;
        if (it->token == STRING) {
            key = it->str;
        } else if (it->token == ESCAPE_STRING) {
            escList.emplace_back(FormString(it->str));
            key = escList.back();
        } else {
            throw std::runtime_error("expected string");
        }
        key.remove_prefix(1);
        key.remove_suffix(1);
        ++it;

        if (it->token != COLON)
            throw std::runtime_error("expected colon");

        ++it;

        JsonValue res = Profile("Parse Value", &JsonParser::ProfileValue, this);
        obj.emplace_back(key, std::move(res));

        isComma = it->token == COMMA;
        it += isComma;
        bannedComma = (it->token == R_BRACE) && isComma;
    }

    if (it->token != R_BRACE)
        throw std::runtime_error("expected }");

    if (bannedComma)
        throw std::runtime_error("comma before } is banned");

    ++it;
    return obj;
}

JsonArray JsonParser::ParseArray() {
    JsonArray res{};
    bool isComma = true, bannedComma = false;
    while (it->token != R_BRACKET && !bannedComma) {
        res.emplace_back(ParseValue());
        isComma = it->token == COMMA;
        it += isComma;
        bannedComma = (it->token == R_BRACKET) && isComma;
    }

    if (it->token != R_BRACKET)
        throw std::runtime_error("expected ]");

    if (bannedComma)
        throw std::runtime_error("comma before ] is banned");

    ++it;
    return res;
}



JsonArray JsonParser::ProfileArray() {
    JsonArray res{};
    bool isComma = true, bannedComma = false;
    while (it->token != R_BRACKET && !bannedComma) {
        res.emplace_back(Profile("ParseValue", &JsonParser::ProfileValue, this));
        isComma = it->token == COMMA;
        it += isComma;
        bannedComma = (it->token == R_BRACKET) && isComma;
    }

    if (it->token != R_BRACKET)
        throw std::runtime_error("expected ]");

    if (bannedComma)
        throw std::runtime_error("comma before ] is banned");

    ++it;
    return res;
}

JsonValue JsonParser::ProfileValue() {
    JsonValue res;
    std::string_view buff;
    switch (it ->token) {
    case L_BRACE:
        ++it;
        res.data = Profile("Parse Object", &JsonParser::ProfileObject, this);
        break;
    case L_BRACKET:
        ++it;
        res.data = Profile("Parse Array", &JsonParser::ProfileArray, this);
        break;
    case STRING:
        it->str.remove_suffix(1);
        it->str.remove_prefix(1);
        res.data = it->str;
        ++it;
        break;
    case ESCAPE_STRING:
        escList.emplace_back(FormString(it->str));
        buff = escList.back();
        buff.remove_prefix(1);
        buff.remove_suffix(1);
        res.data = buff;
        ++it;
        break;
    case NUM: {
        ulonglong buff;
        std::from_chars(it->str.data(), it->str.data() + it->str.size(), buff, 10);
        res.data = buff;
        ++it;
        break;
    }
    case MINUS_NUM: {
        longlong buff;
        std::from_chars(it->str.data(), it->str.data() + it->str.size(), buff, 10);
        res.data = buff;
        ++it;
        break;
    }
    case FRAC:
    case EXP:
    case EXP_MINUS:
    case EXP_FRAC:
    case EXP_FRAC_MINUS:
    case MINUS_FRAC:
    case MINUS_EXP:
    case MINUS_EXP_MINUS:
    case MINUS_EXP_FRAC:
    case MINUS_EXP_FRAC_MINUS: {
        double buff;
        std::from_chars(it->str.data(), it->str.data() + it->str.size(), buff);
        res.data = buff;
        ++it;
        break;
    }
    case TRUE_FOUND:
        res.data = true;
        ++it;
        break;
    case FALSE_FOUND:
        res.data = false;
        ++it;
        break;
    case NULL_FOUND:
        res.data = std::monostate{};
        ++it;
        break;
    default:
        throw std::runtime_error("expected object, array, true, false, number or null");
    }
    return res;
}





JsonValue JsonParser::ParseValue() {
    JsonValue res;
    std::string_view buff;
    switch (it ->token) {
    case L_BRACE:
        ++it;
        res.data = ParseObject();
        break;
    case L_BRACKET:
        ++it;
        res.data = ParseArray();
        break;
    case STRING:
        it->str.remove_suffix(1);
        it->str.remove_prefix(1);
        res.data = it->str;
        ++it;
        break;
    case ESCAPE_STRING:        
        escList.emplace_back(FormString(it->str));
        buff = escList.back();
        buff.remove_prefix(1);
        buff.remove_suffix(1);
        res.data = buff;
        ++it;
        break;
    case NUM: {
        ulonglong buff;
        std::from_chars(it->str.data(), it->str.data() + it->str.size(), buff, 10);
        res.data = buff;
        ++it;
        break;
    }
    case MINUS_NUM: {
        longlong buff;
        std::from_chars(it->str.data(), it->str.data() + it->str.size(), buff, 10);
        res.data = buff;
        ++it;
        break;
    }
    case FRAC:
    case EXP:
    case EXP_MINUS:
    case EXP_FRAC:
    case EXP_FRAC_MINUS:
    case MINUS_FRAC:
    case MINUS_EXP:
    case MINUS_EXP_MINUS:
    case MINUS_EXP_FRAC:
    case MINUS_EXP_FRAC_MINUS: {
        double buff;
        std::from_chars(it->str.data(), it->str.data() + it->str.size(), buff);
        res.data = buff;
        ++it;
        break;
    }
    case TRUE_FOUND:
        res.data = true;
        ++it;
        break;
    case FALSE_FOUND:
        res.data = false;
        ++it;
        break;
    case NULL_FOUND:
        res.data = std::monostate{};
        ++it;
        break;
    default:
        throw std::runtime_error("expected object, array, true, false, number or null");
}

return res;
}

JsonParser Parse(std::vector<MetaToken>&& tokens) {
    JsonParser parser{std::move(tokens)};
    parser();
    return parser;

}

JsonParser ProfileParse(std::vector<MetaToken>&& tokens) {
    JsonParser parser{std::move(tokens)};
    parser.ast = Profile("Parse value", &JsonParser::ProfileValue, &parser);
    return parser;

}

JsonParser ParseJson(const std::string& buffer) {
    std::vector<MetaToken> tokens = Tokenize(buffer);
    JsonParser parser = Parse(std::move(tokens));
    return parser;
}


void JsonParser::Clear() {
    tokens.clear();
    tokens.shrink_to_fit();
}