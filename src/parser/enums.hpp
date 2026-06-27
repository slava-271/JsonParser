#ifndef ENUMS_H
#define ENUMS_H
#include <type_traits>
#include <array>
#include <string>

using uchar = unsigned char;

enum Tokens : uchar {
    L_BRACKET,
    R_BRACKET,
    L_BRACE,
    R_BRACE,
    COLON,
    COMMA,

    STRING,
    ESCAPE_STRING,
    NULL_FOUND,
    TRUE_FOUND,
    FALSE_FOUND,

    NUM,
    FRAC,
    EXP,
    EXP_MINUS,
    EXP_FRAC,
    EXP_FRAC_MINUS,

    MINUS_NUM,
    MINUS_FRAC,
    MINUS_EXP,
    MINUS_EXP_MINUS,
    MINUS_EXP_FRAC,
    MINUS_EXP_FRAC_MINUS,

    ERRORS,

    ERR_NONCHAR,
    ERR_ESC,
    ERR_HEX,

    ERR_ZERO,
    ERR_DIG,
    ERR_DIG_OR_SIGN,

    ERR_BANNED,

    ERR_EOF,

};

constexpr std::array<std::string_view, ERRORS> tokArr {
    "L_BRACKET",
    "R_BRACKET",
    "L_BRACE",
    "R_BRACE",
    "COLON",
    "COMMA",
    "STRING",
    "ESCAPE_STRING",
    "NULL_FOUND",
    "TRUE_FOUND",
    "FALSE_FOUND",
    "NUM",
    "FRAC",
    "EXP",
    "EXP_MINUS",
    "EXP_FRAC",
    "EXP_FRAC_MINUS",
    "MINUS_NUM",
    "MINUS_FRAC",
    "MINUS_EXP",
    "MINUS_EXP_MINUS",
    "MINUS_EXP_FRAC",
    "MINUS_EXP_FRAC_MINUS"
};
// naming of states represents the char needed for the state in DFA or logical phase in DFA
namespace String {
enum States : uchar {
    START,
    CHAR,

    ESC_SEQ,
    HEX1,
    HEX2,
    HEX3,
    HEX4,

    CHAR_ESC,

    COUNT,

    STR_FOUND,
    ESC_STR_FOUND,

    ERRORS,

    ERR_NONCHAR,
    ERR_ESC,
    ERR_HEX,

    END
};
}

namespace Number {
enum States: uchar {
    START,
    ZERO,
    DIG,

    FRAC,
    FRAC_DIG,

    EXP,
    EXP_PLUS,
    EXP_MINUS,
    EXP_DIG,
    EXP_MINUS_DIG,

    FRAC_EXP,
    FRAC_EXP_PLUS,
    FRAC_EXP_MINUS,
    FRAC_EXP_DIG,
    FRAC_EXP_MINUS_DIG,

    COUNT,

    NUM_FOUND,
    FRAC_FOUND,
    EXP_FOUND,
    EXP_MINUS_FOUND,
    FRAC_EXP_FOUND,
    FRAC_EXP_MINUS_FOUND,

    ERRORS,

    ERR_ZERO,
    ERR_DIG,
    ERR_DIG_OR_SIGN,

    END
};
}

namespace Negative {
enum States: uchar {
    START,

    ZERO,
    DIG,

    FRAC,
    FRAC_DIG,

    EXP,
    EXP_PLUS,
    EXP_MINUS,
    EXP_DIG,
    EXP_MINUS_DIG,

    FRAC_EXP,
    FRAC_EXP_PLUS,
    FRAC_EXP_MINUS,
    FRAC_EXP_DIG,
    FRAC_EXP_MINUS_DIG,

    COUNT,


    NUM_FOUND,
    FRAC_FOUND,
    EXP_FOUND,
    EXP_MINUS_FOUND,
    FRAC_EXP_FOUND,
    FRAC_EXP_MINUS_FOUND,

    ERRORS,

    ERR_ZERO,
    ERR_DIG,
    ERR_DIG_OR_SIGN,

    END
};
}

namespace ws {
enum States : uchar {
    START,
    COUNT,
    WS_FOUND
};
}


template<typename EnumType>
concept EnumWithCount = std::is_enum_v<EnumType> && requires {EnumType::COUNT;};

template<typename EnumType>
concept EnumStr = std::is_enum_v<EnumType> &&
                  requires {EnumType::LITERAL && EnumType::FOUND && EnumType::ERR_BANED;};


template <typename T>
concept NumericEnum = std::is_enum_v<T> && requires {
    T::COUNT;
    T::ERRORS;

    T::START;
    T::DIG;
    T::FRAC;
    T::FRAC_DIG;
    T::EXP;
    T::EXP_PLUS;
    T::EXP_MINUS;
    T::EXP_DIG;
    T::EXP_MINUS_DIG;
    T::FRAC_EXP;
    T::FRAC_EXP_PLUS;
    T::FRAC_EXP_MINUS;
    T::FRAC_EXP_DIG;
    T::FRAC_EXP_MINUS_DIG;

    T::NUM_FOUND;
    T::FRAC_FOUND;
    T::EXP_FOUND;
    T::EXP_MINUS_FOUND;
    T::FRAC_EXP_FOUND;
    T::FRAC_EXP_MINUS_FOUND;
};

template<EnumWithCount enumType>
consteval enumType& operator++(enumType& state) {
    state = static_cast<enumType>(static_cast<uchar>(state) + 1);
    return state;
};


#endif // ENUMS_H
