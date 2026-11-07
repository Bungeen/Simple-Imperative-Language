#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "pattern.h"
#include "json_format.h"


class WildcardPattern : public Pattern {
public:
    WildcardPattern() {}

    bool check(Interpreter& interpreter, const Value& value) const override {
        return interpreter.Visit(*this, value);
    }

    void show(std::ostream& os) const {
        os << json_format::PATTERN << ": {\"" << json_format::WILDCARD << "\": []}";
    }
};