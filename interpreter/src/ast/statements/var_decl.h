#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "json_format.h"

class VarDecl : public Stmt {
public:
    VarDecl(std::string name, ExprPtr init = nullptr)
        : name_(std::move(name))
        , init_(std::move(init))
    {}

    const std::string& get_name() const {
        return name_;
    }

    bool has_init() const {
        return init_ != nullptr;
    }

    const Expr& get_init() const {
        return *init_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }

    void show(std::ostream& os) const override {
        os << "{\"" << json_format::VAR << "\":{\"" << json_format::NAME << "\":\"" << name_ << "\"";
        if (init_) {
            os << ",\"" << json_format::VALUE << "\":";
            init_->show(os);
        }
        os << "}}";
    }
private:
    std::string name_;
    ExprPtr init_;
};
