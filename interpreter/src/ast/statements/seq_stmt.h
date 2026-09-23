#pragma once

#include <ostream>
#include <utility>

#include "stmt.h"
#include "json_format.h"


class SeqStmt : public Stmt {
public:
    SeqStmt(StmtPtr lhs, StmtPtr rhs)
        : lhs_(std::move(lhs))
        , rhs_(std::move(rhs))
    {}

    const Stmt& get_lhs() const {
        return *lhs_;
    }

    const Stmt& get_rhs() const {
        return *rhs_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::SEQ << "\":{\"" << json_format::LEFT << "\":";
        lhs_->show(os);
        os << ",\"" << json_format::RIGHT << "\":";
        rhs_->show(os);
        os << "}}";
    }
private:
    StmtPtr lhs_;
    StmtPtr rhs_;
};
