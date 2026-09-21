#pragma once

#include "stmt.h"
#include "expression.h"
#include "json_format.h"

#include <ostream>
#include <utility>


class DoWhileStmt : public Stmt {
private:
    StmtPtr body;
    ExprPtr cond;
public:
    DoWhileStmt(StmtPtr body, ExprPtr cond)
        : body(std::move(body))
        , cond(std::move(cond))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::DO_WHILE << "\":{\"" << json_format::BODY << "\":";
        body->show(os);
        os << ",\"" << json_format::COND << "\":";
        cond->show(os);
        os << "}}";
    }
};
