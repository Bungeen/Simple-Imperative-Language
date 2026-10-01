#pragma once

#include <string_view>

#include "bin_op.h"

struct Operator {
    std::string_view text;
    BinOp operation;
    int precedence;
};

constexpr Operator kOperators[] = {
    {"!!", BinOp::Or, 1},
    {"&&", BinOp::And, 2},
    {"==", BinOp::Equal, 3},
    {"!=", BinOp::NotEqual, 3},
    {"<", BinOp::Less, 4},
    {"<=", BinOp::LessEqual, 4},
    {">", BinOp::Greater, 4},
    {">=", BinOp::GreaterEqual, 4},
    {"+", BinOp::Add, 5},
    {"-", BinOp::Subtract, 5},
    {"*", BinOp::Multiply, 6},
    {"/", BinOp::Divide, 6},
    {"%", BinOp::Modulo, 6}
};
