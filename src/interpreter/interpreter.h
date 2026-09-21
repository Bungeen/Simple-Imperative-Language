#pragma once

#include "ast.h"

#include <istream>
#include <map>
#include <ostream>

class Interpreter {
public:
    Interpreter(std::istream& input, std::ostream& output);

    std::int64_t evaluate(const Expr& expression);
    void execute(const Stmt& statement);

    std::int64_t get_variable(const std::string& name) const;

private:
    std::map<std::string, std::int64_t> variables;
    std::istream& input;
    std::ostream& output;

    std::int64_t apply(BinOp op, std::int64_t left, const Expr& right);
};
