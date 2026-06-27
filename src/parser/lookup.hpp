#ifndef LOOKUP_H
#define LOOKUP_H
#include <array>
#include "charSet.hpp"

template<EnumWithCount StateType>
struct TransitionTable {
    using Row = std::array<uchar, charNum>;
    using Table = std::array<Row, static_cast<size_t>(StateType::COUNT)>;
    Table table;

    class ProxyRow {
        Row& row;
    public:
        constexpr ProxyRow(Row& r): row(r) {};

        constexpr uchar& operator[](const size_t i) {
            return row[i];
        }

        class AssignProxy {
            Row& row;
            CharSet set;
        public:
            constexpr AssignProxy(const CharSet set, Row& row) : row(row), set(set){};

            constexpr void operator=(const uchar state) {
                for (size_t i = 0; i < charNum; ++i) {
                    if (set[i])
                        row[i] = static_cast<StateType>(state);
                }
            }
        };

        constexpr AssignProxy operator[](const CharSet set) {
            return AssignProxy(set, row);
        }
    };

    class ConstProxyRow {
        const Row& row;
    public:
        constexpr ConstProxyRow(const Row& r): row(r) {};

        constexpr const uchar& operator[](const size_t i) const {
            return row[i];
        }

    };

    constexpr ProxyRow operator[](const uchar index) {
        return table[static_cast<StateType>(index)];
    }

    constexpr ConstProxyRow operator[](const uchar index) const {
        return table[static_cast<StateType>(index)];
    }
};

template<EnumWithCount StateType>
struct TableBuilderBase {
    using Table = TransitionTable<StateType>;
    using TokenTable = std::array<Tokens, ERR_EOF>;
    using State = StateType;
};

template<EnumWithCount StateType>
constexpr std::array<Tokens, ERR_EOF> BuildToken(uchar token,const uchar errTok) {
    std::array<Tokens, ERR_EOF> tokenTable{};
    uchar state = StateType::COUNT + 1;
    while (state < StateType::ERRORS) {
        tokenTable[state++] = static_cast<Tokens>(token++);
    }
    ++state;
    token = errTok;
    while (state < StateType::END) {
        tokenTable[state++] = static_cast<Tokens>(token++);
    };
    return tokenTable;
};

template<typename State>
struct Builder;

template<NumericEnum State>
struct Builder<State> {
    using Table = typename TableBuilderBase<State>::Table;
    using TokenTable = typename TableBuilderBase<State>::TokenTable;
    static consteval Table BuildTable() {
        Table lookup{};

        constexpr auto START         = State::START;
        constexpr auto ZERO         = State::ZERO;
        constexpr auto DIG           = State::DIG;
        constexpr auto FRAC          = State::FRAC;
        constexpr auto FRAC_DIG      = State::FRAC_DIG;
        constexpr auto EXP           = State::EXP;
        constexpr auto EXP_PLUS      = State::EXP_PLUS;
        constexpr auto EXP_MINUS     = State::EXP_MINUS;
        constexpr auto EXP_DIG       = State::EXP_DIG;
        constexpr auto EXP_MINUS_DIG = State::EXP_MINUS_DIG;
        constexpr auto FRAC_EXP      = State::FRAC_EXP;
        constexpr auto FRAC_EXP_PLUS = State::FRAC_EXP_PLUS;
        constexpr auto FRAC_EXP_MINUS= State::FRAC_EXP_MINUS;
        constexpr auto FRAC_EXP_DIG  = State::FRAC_EXP_DIG;
        constexpr auto FRAC_EXP_MINUS_DIG = State::FRAC_EXP_MINUS_DIG;

        constexpr auto NUM_FOUND     = State::NUM_FOUND;
        constexpr auto FRAC_FOUND    = State::FRAC_FOUND;
        constexpr auto EXP_FOUND     = State::EXP_FOUND;
        constexpr auto EXP_MINUS_FOUND = State::EXP_MINUS_FOUND;
        constexpr auto FRAC_EXP_FOUND = State::FRAC_EXP_FOUND;
        constexpr auto FRAC_EXP_MINUS_FOUND = State::FRAC_EXP_MINUS_FOUND;

        constexpr auto ERR_DIG = State::ERR_DIG;
        constexpr auto ERR_DIG_OR_SIGN = State::ERR_DIG_OR_SIGN;
        constexpr auto ERR_ZERO = State::ERR_ZERO;


        lookup[START]['0'] = ZERO;
        lookup[START][oneLine] = DIG;

        lookup[ZERO][nonDigit] = NUM_FOUND;
        lookup[ZERO][digits] = ERR_ZERO;
        lookup[ZERO]['.'] = FRAC;
        lookup[ZERO][expChars] = EXP;

        lookup[DIG][nonDigit] = NUM_FOUND;
        lookup[DIG][digits] = DIG;
        lookup[DIG]['.'] = FRAC;
        lookup[DIG][expChars] = EXP;

        lookup[FRAC][nonDigit] = ERR_DIG;
        lookup[FRAC][digits] = FRAC_DIG;

        lookup[FRAC_DIG][nonDigit] = FRAC_FOUND;
        lookup[FRAC_DIG][digits] = FRAC_DIG;
        lookup[FRAC_DIG][expChars] = FRAC_EXP;


        auto setExponent = [&] (
            State s_start, State s_plus, State s_minus,
            State s_dig, State s_minus_dig,
            State f_found, State f_minus_found
            ) {
            lookup[s_start][nonDigSign] = ERR_DIG_OR_SIGN;
            lookup[s_start]['+'] = s_plus;
            lookup[s_start]['-'] = s_minus;
            lookup[s_start][digits] = s_dig;

            lookup[s_plus][nonDigit] = ERR_DIG;
            lookup[s_plus][digits] = s_dig;

            lookup[s_minus][nonDigit] = ERR_DIG;
            lookup[s_minus][digits] = s_minus_dig;

            lookup[s_dig][nonDigit] = f_found;
            lookup[s_dig][digits] = s_dig;

            lookup[s_minus_dig][nonDigit] = f_minus_found;
            lookup[s_minus_dig][digits] = s_minus_dig;
        };

        setExponent(
            EXP, EXP_PLUS, EXP_MINUS,
            EXP_DIG, EXP_MINUS_DIG,
            EXP_FOUND, EXP_MINUS_FOUND
            );

        setExponent(
            FRAC_EXP, FRAC_EXP_PLUS, FRAC_EXP_MINUS,
            FRAC_EXP_DIG, FRAC_EXP_MINUS_DIG,
            FRAC_EXP_FOUND, FRAC_EXP_MINUS_FOUND
            );
        return lookup;
    }

    static consteval TokenTable BuildTokenTable() {
        TokenTable tokenTable{};
        uchar state = State::NUM_FOUND;
        uchar token;
        if constexpr (std::is_same_v<State, Number::States>)
            return BuildToken<Number::States>(NUM, ERR_DIG);
        else
            return BuildToken<Negative::States>(MINUS_NUM, ERR_ZERO);
        return tokenTable;
    }
};

template<>
struct Builder<String::States> : TableBuilderBase<String::States> {
    static consteval Table BuildTable() {
        Table lookup{};
        using enum String::States;

        for (uchar i = START; i < COUNT; ++i)
            lookup[static_cast<State>(i)][notChars] = ERR_NONCHAR;

        lookup[START]['"'] = CHAR;

        lookup[CHAR][chars] = CHAR;
        lookup[CHAR]['"'] = STR_FOUND,
        lookup[CHAR]['\\'] = ESC_SEQ;


        lookup[ESC_SEQ][escChars] = CHAR_ESC;
        lookup[ESC_SEQ]['u'] = HEX1;
        lookup[ESC_SEQ][notEscChars] = ERR_ESC;


        for (uchar i = HEX1; i <= HEX3; ++i) {
            lookup[i][notHexChars] = ERR_HEX;
            lookup[i][hexChars] = static_cast<State>(i + 1);
        }

        lookup[HEX4][hexChars] = CHAR_ESC;
        lookup[HEX4][notHexChars] = ERR_HEX;

        lookup[CHAR_ESC][chars] = CHAR_ESC;
        lookup[CHAR_ESC]['\\'] = ESC_SEQ;
        lookup[CHAR_ESC]['"'] = ESC_STR_FOUND;

        return lookup;
    };

    static consteval TokenTable BuildTokenTable() {
        return BuildToken<String::States>(STRING, ERR_NONCHAR);
    }
};

template<typename StateType>
struct LookupTable;


template<EnumWithCount enumType>
struct LookupTable<enumType> {
    using Table = typename Builder<enumType>::Table;
    using TokenTable = typename Builder<enumType>::TokenTable;
    static constexpr Table lookup = Builder<enumType>::BuildTable();
    static constexpr TokenTable toToken = Builder<enumType>::BuildTokenTable();
    LookupTable() = delete;
    LookupTable(const LookupTable&) = delete;
    LookupTable& operator=(LookupTable) = delete;

};




#endif // LOOKUP_H
