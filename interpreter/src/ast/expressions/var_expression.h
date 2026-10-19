#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "expression.h"
#include "lvalue_expression.h"
#include "json_format.h"


class Var : public LvalueExpr {
public:
    Var(std::string name) 
        : name_(std::move(name))
    {}

    Value evaluate(Interpreter& interpreter) const override {
        return interpreter.Visit(*this);
    }

    Value& evaluate_ref(Interpreter& interpreter) const override {
        return interpreter.VisitRef(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::VAR << "\":\"" << name_ << "\"}";
    }

    const std::string& get_var_name() const {
        return name_;
    }
private:
    std::string name_;
};
