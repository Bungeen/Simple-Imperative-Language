#pragma once

#include <cstdint>
#include <ostream>

#include "expression.h"
#include "json_format.h"
#include "value.h"
#include "overload.h"


class Const : public Expr {
public:
    Const(Value value) 
        : value_(value)
    {}

    const Value& get_value() const  {
        return value_;
    }

    Value evaluate(Interpreter& interpreter) const override {
        return interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        auto to_string = Overload(
            [](auto value)->std::string {
                return "inconversible to string type";
            },
            [](std::shared_ptr<std::string> str)->std::string {
                return *str;
            },
            [](std::int64_t num)->std::string {
                return std::to_string(num);
            }
        );
        os << "{\"" << json_format::CONST << "\":" << std::visit(to_string, value_) << '}';
    }
private:
    const Value value_;
};
