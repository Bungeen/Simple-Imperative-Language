#pragma once

#include <cstddef>
#include <string_view>

enum class Kind {
    Identifier,
    Number,
    Symbol,
    Keyword,
    End
};

struct Token {
    Kind kind;
    std::string_view text;
    std::size_t line;
    std::size_t column;
};
