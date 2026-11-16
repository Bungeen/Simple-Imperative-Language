#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

#include "pattern.h"
#include "json_format.h"
#include "value.h"


class SexpPattern : public Pattern {
public:
    SexpPattern(std::string tag, std::vector<PatternPtr> patterns) 
        : tag_(std::move(tag))
        , patterns_(std::move(patterns))
    {}

    const std::vector<PatternPtr>& get_patterns() const  {
        return patterns_;
    }

    const std::string& get_tag() const  {
        return tag_;
    }

    bool check(Interpreter& interpreter, const Value& value) const override {
        return interpreter.Visit(*this, value);
    }

    void show(std::ostream& os) const {
        os << json_format::PATTERN << ": ";
        os << "{\"" << json_format::SEXP << "\": \"" << tag_ << "\", \"" << json_format::ARGS << "\": [";
        if (!patterns_.empty())
            patterns_[0]->show(os);

        for (std::size_t i = 1; i < patterns_.size(); ++i) {
            os << ",";
            patterns_[i]->show(os);
        }
        os << "]}";
    }
private:
    std::string tag_;
    std::vector<PatternPtr> patterns_;
};