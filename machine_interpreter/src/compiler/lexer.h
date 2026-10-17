#pragma once

#include <cstddef>
#include <string_view>

#include "token.h"

class Lexer {
public:
    Lexer(std::string_view source);

    Token next();

private:
    void skip_ignored();
    bool starts(std::string_view text) const;
    void advance();

    std::string_view source_;
    std::size_t position_ = 0;
    std::size_t line_ = 1;
    std::size_t column_ = 1;
};
