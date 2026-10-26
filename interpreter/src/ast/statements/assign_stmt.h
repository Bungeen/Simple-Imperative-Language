#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "json_format.h"


class AssignStmt : public Stmt {
public:
    AssignStmt(std::string dst_name, ExprPtr src)
        : dst_name_(std::move(dst_name))
        , src_(std::move(src))
    {}

    const std::string& get_dst_name() const {
        return dst_name_;
    }

    const Expr& get_src() const {
        return *src_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::ASSN << "\":{\"" << json_format::DST << "\":\"" << dst_name_;
        os << "\",\"" << json_format::SRC << "\":";
        src_->show(os);
        os << "}}";
    }
private:
    std::string dst_name_;
    ExprPtr src_;
};
