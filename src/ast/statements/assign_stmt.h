#pragma once

#include "stmt.h"
#include "expression.h"
#include "json_format.h"

#include <ostream>
#include <string>
#include <utility>


class AssignStmt : public Stmt {
private:
    std::string dst;
    ExprPtr src;
public:
    AssignStmt(std::string dst, ExprPtr src)
        : dst(std::move(dst))
        , src(std::move(src))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::ASSN << "\":{\"" << json_format::DST << "\":\"" << dst << "\",\""
        << json_format::SRC << "\":";
        src->show(os);
        os << "}}";
    }
};
