#include <stdexcept>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include <memory>

#include "bin_op_executor.h"
#include "bin_op.h"
#include "value.h"

Value BinOpExecutor::ExecuteBinOp(BinOp operation, const Value& lhs, const Value& rhs) {
    return std::invoke(kBinOpMap_.at(operation), this, lhs, rhs);
}

Value BinOpExecutor::Sum(const Value& lhs, const Value& rhs) {
    auto operations = Overload{
        [](auto lhs, auto rhs) -> Value {
            throw std::logic_error("Invalid types for sum");
        },
        [](std::int64_t lhs, std::int64_t rhs) -> Value {
            return lhs + rhs;
        },
        [](const std::shared_ptr<std::string>& lhs, 
           const std::shared_ptr<std::string>& rhs) -> Value {
            return std::make_shared<std::string>(*lhs + *rhs);
        },
        [](const std::shared_ptr<List>& lhs, 
           const std::shared_ptr<List>& rhs) -> Value {
            std::vector<Value> res(lhs->begin(), lhs->end());
            res.insert(res.end(), rhs->begin(), rhs->end()); 
            return std::make_shared<List>(std::move(res));
        }
    };

    return std::visit(operations, lhs, rhs);
}

Value BinOpExecutor::Sub(const Value& lhs, const Value& rhs) {
    auto operations = Overload{
        [](auto lhs, auto rhs) -> Value {
            throw std::logic_error("Invalid types for sub");
        },
        [](std::int64_t lhs, std::int64_t rhs) -> Value {
            return lhs - rhs;
        },
        [](const std::shared_ptr<std::string>& lhs, 
           const std::shared_ptr<std::string>& rhs) -> Value {
            if (lhs->ends_with(*rhs)) 
                return std::make_shared<std::string>(lhs->substr(0, lhs->size() - rhs->size()));
            return lhs;
        }
    };

    return std::visit(operations, lhs, rhs);
}

Value BinOpExecutor::Mult(const Value& lhs, const Value& rhs) {
    auto operations = Overload{
        [](auto lhs, auto rhs) -> Value {
            throw std::logic_error("Invalid types for mult");
        },
        [](std::int64_t lhs, std::int64_t rhs) -> Value {
            return lhs * rhs;
        },
    };

    return std::visit(operations, lhs, rhs);
}

Value BinOpExecutor::Div(const Value& lhs, const Value& rhs) {
    auto operations = Overload{
        [](auto lhs, auto rhs) -> Value {
            throw std::logic_error("Invalid types for div");
        },
        [](std::int64_t lhs, std::int64_t rhs) -> Value {
            if (rhs == 0)
                throw std::runtime_error("Division by zero");
            return lhs / rhs;
        }
    };

    return std::visit(operations, lhs, rhs);
}

Value BinOpExecutor::Remainder(const Value& lhs, const Value& rhs) {
    auto operations = Overload{
        [](auto lhs, auto rhs) -> Value {
            throw std::logic_error("Invalid types for remainder");
        },
        [](std::int64_t lhs, std::int64_t rhs) -> Value {
            return lhs % rhs;
        }
    };

    return std::visit(operations, lhs, rhs);
}

std::int64_t BinOpExecutor::Compare(const Value& lhs, const Value& rhs) {
    auto operations = Overload{
        [](auto lhs, auto rhs) -> std::int64_t {
            throw std::logic_error("Invalid types for compare");
        },
        [](std::int64_t lhs, std::int64_t rhs) -> std::int64_t {
            return lhs - rhs;
        },
        [](const std::shared_ptr<std::string>& lhs, const std::shared_ptr<std::string>& rhs) -> std::int64_t {
            return lhs->compare(*rhs);
        }
    };

    return std::visit(operations, lhs, rhs);
}

Value BinOpExecutor::Equal(const Value& lhs, const Value& rhs) {
    return static_cast<std::int64_t>(Compare(lhs, rhs) == 0);
}

Value BinOpExecutor::NotEqual(const Value& lhs, const Value& rhs) {
    return static_cast<std::int64_t>(Compare(lhs, rhs) != 0);
}

Value BinOpExecutor::Less(const Value& lhs, const Value& rhs) {
    return static_cast<std::int64_t>(Compare(lhs, rhs) < 0);
}

Value BinOpExecutor::Greater(const Value& lhs, const Value& rhs) {
    return static_cast<std::int64_t>(Compare(lhs, rhs) > 0);
}

Value BinOpExecutor::LessEqual(const Value& lhs, const Value& rhs) {
    return static_cast<std::int64_t>(Compare(lhs, rhs) <= 0);
}

Value BinOpExecutor::GreaterEqual(const Value& lhs, const Value& rhs) {
    return static_cast<std::int64_t>(Compare(lhs, rhs) >= 0);
}

Value BinOpExecutor::And(const Value& lhs, const Value& rhs) {
    return static_cast<std::int64_t>(lhs.to_bool() && rhs.to_bool());
}

Value BinOpExecutor::Or(const Value& lhs, const Value& rhs) {
    return static_cast<std::int64_t>(lhs.to_bool() || rhs.to_bool());
}
