#pragma once

#include "stmt.h"
#include "json_format.h"

#include <ostream>
#include <utility>


class SeqStmt : public Stmt {
public:
    StmtPtr lhs, rhs;

    SeqStmt(StmtPtr lhs, StmtPtr rhs)
        : lhs(std::move(lhs))
        , rhs(std::move(rhs))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::SEQ << "\":{\"" << json_format::LEFT << "\":";
        lhs->show(os);
        os << ",\"" << json_format::RIGHT << "\":";
        rhs->show(os);
        os << "}}";
    }
};
