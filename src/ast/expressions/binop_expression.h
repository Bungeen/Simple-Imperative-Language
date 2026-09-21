#pragma once

#include "expression.h"
#include "binop.h"
#include "json_format.h"

#include <ostream>
#include <utility>


class BinOpExpr : public Expr {
public:
    BinOp op;
    ExprPtr lhs, rhs;

    BinOpExpr(BinOp op, ExprPtr lhs, ExprPtr rhs)
        : op(op)
        , lhs(std::move(lhs))
        , rhs(std::move(rhs))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::BINOP << "\":\"" << spelling(op) << "\",\"" << json_format::LEFT << "\":";
        lhs->show(os);
        os << ",\"" << json_format::RIGHT << "\":";
        rhs->show(os);
        os << '}';
    }
};
