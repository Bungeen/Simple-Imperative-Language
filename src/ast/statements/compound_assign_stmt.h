#pragma once

#include "stmt.h"
#include "binop.h"
#include "expression.h"
#include "json_format.h"

#include <ostream>
#include <string>
#include <utility>


class CompoundAssignStmt : public Stmt {
private:
    std::string dst;
    BinOp op;
    ExprPtr src;
public:
    CompoundAssignStmt(std::string dst, BinOp op, ExprPtr src)
        : dst(std::move(dst))
        , op(op)
        , src(std::move(src))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::COMPOUND_ASSN << "\":{\"" << json_format::DST << "\":\"" << dst
        << "\",\"" << json_format::BINOP << "\":\"" << spelling(op) << "\",\"" << json_format::SRC
        << "\":";
        src->show(os);
        os << "}}";
    }
};
