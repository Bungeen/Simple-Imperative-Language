#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "pattern.h"
#include "json_format.h"


class StringPattern : public Pattern {
public:
    StringPattern(std::string value) 
        : value_(std::move(value))
    {}

    bool check(Interpreter& interpreter, const Value& value) const override {
        return interpreter.Visit(*this, value);
    }

    void show(std::ostream& os) const {
        os << json_format::PATTERN << ": \"" << value_ << "\"";
    }

    const std::string& get_value() const {
        return value_;
    }
private:
    std::string value_;
};