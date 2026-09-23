#pragma once

#include <cstdint>
#include <ostream>

#include "expression.h"
#include "json_format.h"
#include "value.h"


class Const : public Expr {
public:
    Const(std::int64_t value) 
        : value_(value)
    {}

    const Value& get_value() const  {
        return value_;
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::CONST << "\":" << value_ << '}';
    }
private:
    const Value value_;
};
