#include <stdexcept>

#include <cstdint>

#include "bin_op_executor.h"
#include "bin_op.h"

Value BinOpExecutor::ExecuteBinOp(BinOp operation, const Value& lhs, const Value& rhs) {
    return std::invoke(kBinOpMap_.at(operation), this, lhs, rhs);
}

Value BinOpExecutor::Sum(const Value& lhs, const Value& rhs) {
    return lhs + rhs;
}

Value BinOpExecutor::Sub(const Value& lhs, const Value& rhs) {
    return lhs - rhs;
}

Value BinOpExecutor::Mult(const Value& lhs, const Value& rhs) {
    return lhs * rhs;
}

Value BinOpExecutor::Div(const Value& lhs, const Value& rhs) {
    if (rhs == 0.0)
        throw std::runtime_error("Division by zero");

    return lhs / rhs;
}

Value BinOpExecutor::Remainder(const Value& lhs, const Value& rhs) {
     return lhs % rhs;
}

Value BinOpExecutor::Equal(const Value& lhs, const Value& rhs) {
    return static_cast<Value>(lhs == rhs);
}

Value BinOpExecutor::NotEqual(const Value& lhs, const Value& rhs) {
    return static_cast<Value>(lhs != rhs);
}

Value BinOpExecutor::Less(const Value& lhs, const Value& rhs) {
    return static_cast<Value>(lhs < rhs);
}

Value BinOpExecutor::Greater(const Value& lhs, const Value& rhs) {
    return static_cast<Value>(lhs > rhs);
}

Value BinOpExecutor::LessEqual(const Value& lhs, const Value& rhs) {
    return static_cast<Value>(lhs <= rhs);
}

Value BinOpExecutor::GreaterEqual(const Value& lhs, const Value& rhs) {
    return static_cast<Value>(lhs >= rhs);
}

Value BinOpExecutor::And(const Value& lhs, const Value& rhs) {
    return static_cast<Value>(static_cast<bool>(lhs) && static_cast<bool>(rhs));
}

Value BinOpExecutor::Or(const Value& lhs, const Value& rhs) {
    return static_cast<Value>(static_cast<bool>(lhs) || static_cast<bool>(rhs));
}