#pragma once

#include <ostream>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "json_format.h"


class WhileStmt : public Stmt {
public:
    WhileStmt(ExprPtr cond, StmtPtr body)
        : cond_(std::move(cond))
        , body_(std::move(body))
    {}


    void show(std::ostream& os) const {
        os << "{\"" << json_format::WHILE << "\":{\"" << json_format::COND << "\":";
        cond_->show(os);
        os << ",\"" << json_format::BODY << "\":";
        body_->show(os);
        os << "}}";
    }

    const Expr& get_condition() const {
        return *cond_;
    } 

    const Stmt& get_body() const {
        return *body_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }
private:
    ExprPtr cond_;
    StmtPtr body_;
};
