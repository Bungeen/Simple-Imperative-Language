#pragma once

#include <ostream>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "json_format.h"


class DoWhileStmt : public Stmt {
public:
    DoWhileStmt(StmtPtr body, ExprPtr cond)
        : body_(std::move(body))
        , cond_(std::move(cond))
    {}

    const Expr& get_condition() const {
        return *cond_;
    } 

    const Stmt& get_body() const {
        return *body_;
    } 

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::DO_WHILE << "\":{\"" << json_format::BODY << "\":";
        body_->show(os);
        os << ",\"" << json_format::COND << "\":";
        cond_->show(os);
        os << "}}";
    }
private:
    StmtPtr body_;
    ExprPtr cond_;
};
