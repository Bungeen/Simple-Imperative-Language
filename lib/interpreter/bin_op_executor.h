#pragma once

#include <unordered_map>

#include "bin_op_node.h"
#include "value.h"

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
        {BinOp::sum, &BinOpExecutor::Sum}, 
        {BinOp::sub, &BinOpExecutor::Sub}, 
        {BinOp::mult, &BinOpExecutor::Mult},
        {BinOp::div, &BinOpExecutor::Div}, 
        {BinOp::remainder, &BinOpExecutor::Remainder}, 
        {BinOp::equal, &BinOpExecutor::Equal},
        {BinOp::nequal, &BinOpExecutor::NotEqual},
        {BinOp::less, &BinOpExecutor::Less},
        {BinOp::greater, &BinOpExecutor::Greater},
        {BinOp::less_eq, &BinOpExecutor::LessEqual},
        {BinOp::greater_eq, &BinOpExecutor::GreaterEqual},
        {BinOp::logical_and, &BinOpExecutor::And},
        {BinOp::logical_or, &BinOpExecutor::Or}
    };
};