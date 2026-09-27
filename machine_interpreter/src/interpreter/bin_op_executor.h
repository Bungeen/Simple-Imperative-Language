#pragma once

#include <unordered_map>

#include "bin_op.h"
#include "command.h"

class BinOpExecutor {
public:
    Value ExecuteBinOp(BinOp operation, const Value& lhs, const Value& rhs);
private:
    Value Sum(const Value& lhs, const Value& rhs) ;
    Value Sub(const Value& lhs, const Value& rhs);
    Value Mult(const Value& lhs, const Value& rhs);
    Value Div(const Value& lhs, const Value& rhs);
    Value Remainder(const Value& lhs, const Value& rhs);
    Value Equal(const Value& lhs, const Value& rhs);
    Value NotEqual(const Value& lhs, const Value& rhs);
    Value Less(const Value& lhs, const Value& rhs);
    Value Greater(const Value& lhs, const Value& rhs);
    Value LessEqual(const Value& lhs, const Value& rhs);
    Value GreaterEqual(const Value& lhs, const Value& rhs);
    Value And(const Value& lhs, const Value& rhs);
    Value Or(const Value& lhs, const Value& rhs);

    using BinOpPtr = Value(BinOpExecutor::*)(const Value&, const Value&);

    const std::unordered_map<BinOp, BinOpPtr> kBinOpMap_ = {
        {BinOp::Add, &BinOpExecutor::Sum}, 
        {BinOp::Subtract, &BinOpExecutor::Sub}, 
        {BinOp::Multiply, &BinOpExecutor::Mult},
        {BinOp::Divide, &BinOpExecutor::Div}, 
        {BinOp::Modulo, &BinOpExecutor::Remainder}, 
        {BinOp::Equal, &BinOpExecutor::Equal},
        {BinOp::NotEqual, &BinOpExecutor::NotEqual},
        {BinOp::Less, &BinOpExecutor::Less},
        {BinOp::Greater, &BinOpExecutor::Greater},
        {BinOp::LessEqual, &BinOpExecutor::LessEqual},
        {BinOp::GreaterEqual, &BinOpExecutor::GreaterEqual},
        {BinOp::And, &BinOpExecutor::And},
        {BinOp::Or, &BinOpExecutor::Or}
    };
};