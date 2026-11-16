#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "pattern.h"
#include "json_format.h"


class ArrayPattern : public Pattern {
public:
    ArrayPattern(std::vector<PatternPtr> patterns) 
        : patterns_(std::move(patterns))
    {}

    bool check(Interpreter& interpreter, const Value& value) const override {
        return interpreter.Visit(*this, value);
    }

    void show(std::ostream& os) const {
        os << json_format::PATTERN << ": {\"" << json_format::ARRAY << "\": [";

        if (!patterns_.empty())
            patterns_[0]->show(os);

        for (std::size_t i = 1; i < patterns_.size(); ++i) {
            os << ",";
            patterns_[i]->show(os);
        }
        os << "]";
    }

    const std::vector<PatternPtr>& get_patterns() const {
        return patterns_;
    }
private:
    std::vector<PatternPtr> patterns_;
};