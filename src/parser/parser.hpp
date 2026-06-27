#ifndef PARSER_HPP
#define PARSER_HPP
#include <variant>
#include <utility>
#include <iostream>
#include <charconv>
#include <print>
#include <list>
#include <deque>
#include "tokenize.hpp"

#include "profiler/profiler.hpp"

struct JsonValue;
struct JsonMember;

using JsonArray = std::vector<JsonValue>;
using JsonObject = std::vector<JsonMember>;

using ulong = unsigned long;
using ulonglong = unsigned long long;
using longlong = signed long long;

struct JsonValue {
    std::variant <
        std::monostate,
        bool,
        double,
        longlong,
        ulonglong,
        std::string_view,
        std::string,
        JsonArray,
        JsonObject
        > data;
};

struct JsonMember {
    std::string_view str;
    JsonValue val;
};

class JsonParser {
    std::vector<MetaToken> tokens;
    std::vector<MetaToken>::iterator it = tokens.begin();
    std::list<std::string> escList;

    JsonObject ParseObject();
    JsonArray ParseArray();
    JsonValue ParseValue();
    JsonObject ParseMember();
    JsonValue ParseElement();

    JsonValue ProfileValue();
    JsonArray ProfileArray();
    JsonObject ProfileObject();

    struct PrintVisitor {
        std::string tab{};

        void operator()(std::monostate) const;

        void operator()(const bool val) const;

        void operator()(const double val) const;

        void operator()(const longlong val) const;

        void operator()(const ulonglong val) const;

        void operator()(const std::string_view& val) const;

        void operator()(const std::string& val) const;

        void operator()(const JsonArray& arr);

        void operator()(const JsonObject& obj);

    };

public:
    void operator()();
    JsonParser(const std::vector<MetaToken>& tok) : tokens(tok) {};
    JsonParser(std::vector<MetaToken>&& tok) : tokens(std::move(tok)) {};
    JsonParser(const std::string_view& buffer) : tokens(Tokenize(buffer)) {};
    void Clear();
    void JsonPrint();
    JsonValue ast;

    friend JsonParser Parse(std::vector<MetaToken>&& tokens);

    friend JsonParser ProfileParse(std::vector<MetaToken>&& tokens);
};


JsonParser Parse(std::vector<MetaToken>&& tokens);
JsonParser ProfileParse(std::vector<MetaToken>&& tokens);

JsonParser ParseJson(const std::string& buffer);


#endif // PARSER_HPP
