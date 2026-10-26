#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "command.h"
#include "lexer.h"
#include "operator.h"
#include "token.h"

class Parser {
public:
    Parser(std::string_view source);

    std::vector<Command> program();

private:
    void advance();
    bool accept(std::string_view text);
    void expect(std::string_view text);
    std::string identifier();
    void emit(Operation operation, CommandArgument argument = {});
    std::string label();
    const Operator *binary_operator() const;

    void operand();
    void expression(int minimum_precedence = 1);
    void condition();
    void block();
    void conditional();
    void statement();

    Lexer lexer_;
    Token token_;
    std::vector<Command> commands_;
    std::size_t next_label_ = 0;
};
