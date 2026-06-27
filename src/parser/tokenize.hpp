#ifndef TOKENIZE_H
#define TOKENIZE_H
#include <vector>
#include <stdexcept>
#include "lookup.hpp"

struct MetaToken {
    Tokens token;
    std::string_view str;
    MetaToken(const Tokens tok, const std::string_view::const_iterator& start,
              const  std::string_view::const_iterator& end) : token(tok), str(start, end - start + 1) {};
};

std::vector<MetaToken> Tokenize(const std::string_view str);

#endif // TOKENIZE_H
