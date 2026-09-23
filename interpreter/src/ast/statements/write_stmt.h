#pragma once

#include <ostream>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "json_format.h"


class WriteStmt : public Stmt {
public:
    WriteStmt(ExprPtr value)
        : value_(std::move(value))
    {}

    const Expr& get_value() const {
        return *value_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::WRITE << "\":";
        value_->show(os);
        os << '}';
    }
private:
    ExprPtr value_;
};
