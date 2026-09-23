#pragma once

#include "expression.h"
#include "binop.h"
#include "json_format.h"

#include <ostream>
#include <utility>


class BinOpExpr : public Expr {
public:
    BinOpExpr(BinOp op, ExprPtr lhs, ExprPtr rhs)
        : op_(op)
        , lhs_(std::move(lhs))
        , rhs_(std::move(rhs))
    {}

    const Expr& get_lhs() const {
        return *lhs_;
    } 

    const Expr& get_rhs() const {
        return *rhs_;
    } 

    BinOp get_operation() const {
        return op_;
    }

    Value evaluate(Interpreter& interpreter) const override {
        return interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::BINOP << "\":\"" << spelling(op_) << "\",\"" << json_format::LEFT << "\":";
        lhs_->show(os);
        os << ",\"" << json_format::RIGHT << "\":";
        rhs_->show(os);
        os << '}';
    }
private:
    BinOp op_;
    ExprPtr lhs_;
    ExprPtr rhs_;
};
