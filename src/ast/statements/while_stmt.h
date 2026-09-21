#pragma once

#include "stmt.h"
#include "expression.h"
#include "json_format.h"

#include <ostream>
#include <utility>


class WhileStmt : public Stmt {
private:
    ExprPtr cond;
    StmtPtr body;
public:
    WhileStmt(ExprPtr cond, StmtPtr body)
        : cond(std::move(cond))
        , body(std::move(body))
    {}


    void show(std::ostream& os) const {
        os << "{\"" << json_format::WHILE << "\":{\"" << json_format::COND << "\":";
        cond->show(os);
        os << ",\"" << json_format::BODY << "\":";
        body->show(os);
        os << "}}";
    }
};
