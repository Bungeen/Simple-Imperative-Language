#pragma once

#include "stmt.h"
#include "expression.h"
#include "json_format.h"

#include <ostream>
#include <utility>


class IfStmt : public Stmt {
private:
    ExprPtr cond;
    StmtPtr thenBranch;
    StmtPtr elseBranch;
public:
    IfStmt(ExprPtr cond, StmtPtr thenBranch, StmtPtr elseBranch)
        : cond(std::move(cond))
        , thenBranch(std::move(thenBranch))
        , elseBranch(std::move(elseBranch))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::IF << "\":{\"" << json_format::COND << "\":";
        cond->show(os);
        os << ",\"" << json_format::THEN << "\":";
        thenBranch->show(os);
        if (elseBranch) {
            os << ",\"" << json_format::ELSE << "\":";
            elseBranch->show(os);
        }
        os << "}}";
    }
};
