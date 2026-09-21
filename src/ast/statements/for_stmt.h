#pragma once

#include "stmt.h"
#include "expression.h"
#include "json_format.h"

#include <ostream>
#include <utility>


class ForStmt : public Stmt {
private:
    StmtPtr init;
    ExprPtr cond;
    StmtPtr step, body;
public:
    ForStmt(StmtPtr init, ExprPtr cond, StmtPtr step, StmtPtr body)
        : init(std::move(init))
        , cond(std::move(cond))
        , step(std::move(step))
        , body(std::move(body))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::FOR << "\":{\"" << json_format::INIT << "\":";
        init->show(os);
        os << ",\"" << json_format::COND << "\":";
        cond->show(os);
        os << ",\"" << json_format::STEP << "\":";
        step->show(os);
        os << ",\"" << json_format::BODY << "\":";
        body->show(os);
        os << "}}";
    }
};
