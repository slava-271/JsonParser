#include "tokenize.hpp"

constexpr std::string_view trueStr = "true";
constexpr std::string_view falseStr = "false";
constexpr std::string_view nullStr = "null";

template<EnumWithCount StateType>
Tokens FindToken(std::string_view::const_iterator& it, const std::string_view::const_iterator& cend) {
    uchar state = StateType::START;
    while (it != cend && state < StateType::COUNT) {
        state = LookupTable<StateType>::lookup[state][*it];
        it += (state < StateType::COUNT);
    }

    if (it == cend && state < StateType::COUNT) {
        if constexpr (std::is_same_v<StateType, Negative::States> || std::is_same_v<StateType, Number::States>) {
            state = LookupTable<StateType>::lookup[state][' '];
        } else
            return ERR_EOF;
    }
    return LookupTable<StateType>::toToken[state];

}

std::vector<MetaToken> Tokenize(const std::string_view str) {
    std::vector<MetaToken> res;
    res.reserve(str.size() / 8);
    Tokens token;
    for (auto it = str.cbegin(); it < str.cend(); ++it) {
        auto start = it;
        switch(*it) {
        case '{': token = L_BRACE;
            break;
        case '}': token = R_BRACE;
            break;
        case '[': token = L_BRACKET;
            break;
        case ']': token = R_BRACKET;
            break;
        case ':': token = COLON;
            break;
        case ',': token = COMMA;
            break;
        case '"': token = FindToken<String::States>(it, str.cend());
            break;
        case 't':
            if (str.compare(it - str.cbegin(), trueStr.size(), trueStr) == 0) [[likely]] {
                it += trueStr.size() - 1;
                token = TRUE_FOUND;
            }
            else
                token = ERR_BANNED;
            break;
        case 'n':
            if (str.compare(it - str.cbegin(), nullStr.size(), nullStr) == 0) [[likely]] {
                it += nullStr.size() - 1;
                token = NULL_FOUND;
            }
            else
                token = ERR_BANNED;
            break;
        case 'f':
            if (str.compare(it - str.cbegin(), falseStr.size(), falseStr) == 0) [[likely]] {
                it += falseStr.size() - 1;
                token = FALSE_FOUND;
            }
            else
                token = ERR_BANNED;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': token = FindToken<Number::States>(it, str.cend());
            break;
        case '-':
            ++it;
            token = FindToken<Negative::States>(it, str.cend());
            break;
        case 0x20:
        case 0x0a:
        case 0x0d:
        case 0x09:
            do {
                ++it;
            } while (it != str.cend() && (*it == 0x20 || *it == 0x0a || *it == 0x0d || *it == 0x09));
            --it;
            continue;
        default:
            // std::string err = "banned symbol: '";
            // if (static_cast<uchar>(*it) >= 32 && static_cast<uchar>(*it) <= 126) {
            //     err.push_back(*it); // Если печатный символ
            // } else {
            //     err += "\\x"; // Если непечатный (битый байт)
            // }
            // err += "' (ASCII code: " + std::to_string(static_cast<int>(static_cast<uchar>(*it))) + ")";
            // err += " at position " + std::to_string(std::distance(str.cbegin(), it));

            token = ERR_BANNED;
            throw std::runtime_error("banned symbol");
        }

        it -= (token >= NUM && token <= MINUS_EXP_FRAC_MINUS);

        if (token < ERRORS)
            res.emplace_back(token, start, it);
        else {
            std::string error{};
            switch (token) {
            case ERR_NONCHAR:
                error = "expected char";
                break;
            case ERR_ESC:
                error = "expected escape char";
                break;
            case ERR_HEX:
                error = "expected hex digit";
                break;
            case ERR_ZERO:
                error = "expected zero";
                break;
            case ERR_DIG:
                error = "expected digit";
                break;
            case ERR_DIG_OR_SIGN:
                error = "digit or sign";
                break;
            case ERR_BANNED:
                error = "banned symbol";
                break;
            case ERR_EOF:
                error = "end of file";
                break;
            }
            throw std::runtime_error("Lexer Error" + error);
        }

    }

    if ((res[0].token == L_BRACE && res.back().token != R_BRACE))
        throw std::runtime_error("expected } at the end of file");
    else if ((res[0].token == L_BRACKET && res.back().token != R_BRACKET))
        throw std::runtime_error("expected ] at the end of file");

    return res;
}