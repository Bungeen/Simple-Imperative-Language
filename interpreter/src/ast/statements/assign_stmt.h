#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "lvalue_expression.h"
#include "json_format.h"


class AssignStmt : public Stmt {
public:
    AssignStmt(LvalueExprPtr dst, ExprPtr src)
        : dst_(std::move(dst))
        , src_(std::move(src))
    {}

    const LvalueExpr& get_dst() const {
        return *dst_;
    }

    const Expr& get_src() const {
        return *src_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::ASSN << "\":{\"" << json_format::DST << "\":\"";
        dst_->show(os);
        os << "\",\"" << json_format::SRC << "\":";
        src_->show(os);
        os << "}}";
    }
private:
    LvalueExprPtr dst_;
    ExprPtr src_;
};
