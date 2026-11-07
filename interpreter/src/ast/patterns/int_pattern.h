#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "pattern.h"
#include "json_format.h"


class IntPattern : public Pattern {
public:
    IntPattern(std::int64_t value) 
        : value_(value)
    {}

    bool check(Interpreter& interpreter, const Value& value) const override {
        return interpreter.Visit(*this, value);
    }

    void show(std::ostream& os) const {
        os << json_format::PATTERN << ": " << value_;
    }

    int64_t get_value() const {
        return value_;
    }
private:
    std::int64_t value_;
};