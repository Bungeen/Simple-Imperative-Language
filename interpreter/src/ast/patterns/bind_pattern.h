#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "pattern.h"
#include "json_format.h"


class BindPattern : public Pattern {
public:
    BindPattern(std::string name, PatternPtr pattern) 
        : name_(std::move(name))
        , pattern_(std::move(pattern))
    {}

    bool check(Interpreter& interpreter, const Value& value) const override {
        return interpreter.Visit(*this, value);
    }

    void show(std::ostream& os) const {
        os << json_format::PATTERN << ": {\"" << json_format::BIND << "\": {";
        pattern_->show(os);
        os << "}, ";
        os << "\"" << json_format::NAME << "\": \"" << name_ << "\"}";
    }

    const std::string& get_var_name() const {
        return name_;
    }

    const Pattern& get_pattern() const {
        return *pattern_;
    }
private:
    std::string name_;
    PatternPtr pattern_;
};