#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "json_format.h"


class AssignElemStmt : public Stmt {
public:
    AssignElemStmt(ExprPtr dst, ExprPtr idx, ExprPtr src)
        : dst_(std::move(dst))
        , idx_(std::move(idx))
        , src_(std::move(src))
    {}

    const Expr& get_dst() const {
        return *dst_;
    }

    const Expr& get_idx() const {
        return *idx_;
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
    ExprPtr dst_;
    ExprPtr idx_;
    ExprPtr src_;
};
