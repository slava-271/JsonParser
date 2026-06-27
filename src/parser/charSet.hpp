#ifndef CHARSET_HPP
#define CHARSET_HPP
#include <bitset>
#include "enums.hpp"
using uint = unsigned int;
constexpr uint charNum = 256;

class CharSet {
    using BitSet = std::bitset<charNum>;
    BitSet bitset{};
public:
    consteval CharSet(const std::initializer_list<uchar> chars) {
        for (uchar c: chars)
            bitset[c] = 1;
    }

    consteval CharSet(const uchar start, const uchar end)
    {
        for (size_t i = start; i <= end; ++i)
            bitset[i] = 1;
    }

    consteval CharSet() = default;

    constexpr CharSet(const CharSet& set) : bitset(set.bitset) {};

    consteval CharSet(const BitSet& set) : bitset(set) {};






    consteval CharSet operator~() const {
        return ~bitset;
    }

    consteval auto operator[](uchar i) const{
        return bitset[i];
    }

    consteval auto operator[](uchar i) {
        return bitset[i];
    }

    friend consteval CharSet operator+(const CharSet& set1, const CharSet& set2) {
        return set1.bitset | set2.bitset;
    }

    friend consteval CharSet operator|(const CharSet& set1, const CharSet& set2) {
        return set1 + set2;
    }

    friend consteval CharSet operator+(CharSet set, const uchar sym){
        set[sym] = 1;
        return set;
    }

    friend consteval CharSet operator|(CharSet set, const uchar sym){
        return set + sym;
    }

    friend consteval CharSet operator*(const CharSet& set1, const CharSet& set2) {
        return set1.bitset & set2.bitset;
    }

    friend consteval CharSet operator&(const CharSet& set1, const CharSet& set2) {
        return set1 * set2;
    }

    friend consteval CharSet operator-(const CharSet& set1, const CharSet& set2) {
        return set1 & ~set2;
    }

    consteval CharSet operator-(const uchar sym) const {
        CharSet res = *this;
        res[sym] = 0;
        return bitset;
    }


    friend consteval CharSet operator^(const CharSet& set1, const CharSet& set2) {
        return set1.bitset ^ set2.bitset;
    }
};

constexpr CharSet oneLine('1', '9');
constexpr CharSet expChars = {'e', 'E'};

constexpr CharSet digits = oneLine + '0';

constexpr CharSet nonDigit = ~digits;
constexpr CharSet nonDigSign = nonDigit + ~expChars;

constexpr CharSet notChars(0x0, 0x1F);
constexpr CharSet chars = ~notChars - '\\' - '"';


constexpr CharSet escChars = {'"', '\\', '/', 'b', 'f', 'n', 'r', 't'};
constexpr CharSet notEscChars = ~(escChars + 'u');
constexpr CharSet hexChars = digits + CharSet('a', 'f') + CharSet('A', 'F');
constexpr CharSet notHexChars = ~hexChars;

constexpr CharSet wsChars = {0x20, 0x0a, 0x0d, 0x09};

#endif // CHARSET_HPP
