#pragma once

#include <string>
#include <stdexcept>

#include "json_format.h"


enum class BinOp {
    Or,
    And,
    Equal,
    NotEqual,
    LessEqual,
    Less,
    GreaterEqual,
    Greater,
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
};

const BinOp ALL_BINOPS[] = {
    BinOp::Or, BinOp::And,
    BinOp::Equal, BinOp::NotEqual,
    BinOp::LessEqual, BinOp::Less,
    BinOp::GreaterEqual, BinOp::Greater,
    BinOp::Add, BinOp::Subtract,
    BinOp::Multiply, BinOp::Divide, BinOp::Modulo
};

inline const std::string& spelling(BinOp op) {
    switch (op) {
        case BinOp::Or:           return binop_text::OR;
        case BinOp::And:          return binop_text::AND;
        case BinOp::Equal:        return binop_text::EQUAL;
        case BinOp::NotEqual:     return binop_text::NOT_EQUAL;
        case BinOp::LessEqual:    return binop_text::LESS_EQUAL;
        case BinOp::Less:         return binop_text::LESS;
        case BinOp::GreaterEqual: return binop_text::GREATER_EQUAL;
        case BinOp::Greater:      return binop_text::GREATER;
        case BinOp::Add:          return binop_text::ADD;
        case BinOp::Subtract:     return binop_text::SUBTRACT;
        case BinOp::Multiply:     return binop_text::MULTIPLY;
        case BinOp::Divide:       return binop_text::DIVIDE;
        case BinOp::Modulo:       return binop_text::MODULO;
    }
    throw std::invalid_argument("unknown binary operator");
}
