#pragma once

#include "expression.h"
#include "json_format.h"

#include <cstdint>
#include <ostream>


class Const : public Expr {
public:
    const std::int64_t value;

    Const(std::int64_t value) 
        : value(value)
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::CONST << "\":" << value << '}';
    }
};
