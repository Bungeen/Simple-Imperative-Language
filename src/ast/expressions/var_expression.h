#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "expression.h"
#include "json_format.h"


class Var : public Expr {
public:
    Var(std::string name) 
        : name_(std::move(name))
    {}

    Value evaluate(Interpreter& interpreter) const override {
        return interpreter.Visit(*this);
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
