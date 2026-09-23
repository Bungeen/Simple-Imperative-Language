#pragma once

#include <ostream>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "json_format.h"


class IfStmt : public Stmt {
public:
    IfStmt(ExprPtr cond, StmtPtr thenBranch, StmtPtr elseBranch)
        : cond_(std::move(cond))
        , then_branch_(std::move(thenBranch))
        , else_branch_(std::move(elseBranch))
    {}

    const Expr& get_condition() const {
        return *cond_;
    } 

    const Stmt& get_then_branch() const {
        return *then_branch_;
    } 

    const Stmt& get_else_branch() const {
        return *else_branch_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::IF << "\":{\"" << json_format::COND << "\":";
        cond_->show(os);
        os << ",\"" << json_format::THEN << "\":";
        then_branch_->show(os);
        if (else_branch_) {
            os << ",\"" << json_format::ELSE << "\":";
            else_branch_->show(os);
        }
        os << "}}";
    }
private:
    ExprPtr cond_;
    StmtPtr then_branch_;
    StmtPtr else_branch_;
};
