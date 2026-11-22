#pragma once

#include <ostream>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "json_format.h"

class ReturnStmt : public Stmt {
public:
    ReturnStmt(ExprPtr expr = nullptr)
        : expr_(std::move(expr))
    {}

    bool has_expr() const {
        return expr_ != nullptr;
    }

    const Expr& get_expr() const {
        return *expr_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }

    void show(std::ostream& os) const override {
        os << "{\"" << json_format::RETURN << "\":";
        if (expr_) {
            expr_->show(os);
        } else {
            os << "null";
        }
        os << "}";
    }
private:
    ExprPtr expr_;
};
