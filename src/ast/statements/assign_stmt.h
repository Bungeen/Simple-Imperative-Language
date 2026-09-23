#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "json_format.h"


class AssignStmt : public Stmt {
public:
    AssignStmt(std::string dst, ExprPtr src)
        : dst_(std::move(dst))
        , src_(std::move(src))
    {}

    const std::string& get_dst() const {
        return dst_;
    }

    const Expr& get_src() const {
        return *src_;
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::ASSN << "\":{\"" << json_format::DST << "\":\"" << dst_ << "\",\""
        << json_format::SRC << "\":";
        src_->show(os);
        os << "}}";
    }
private:
    std::string dst_;
    ExprPtr src_;
};
