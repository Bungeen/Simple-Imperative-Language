#pragma once

#include "stmt.h"
#include "expression.h"
#include "json_format.h"

#include <ostream>
#include <utility>


class WriteStmt : public Stmt {
private:
    ExprPtr value;
public:
    WriteStmt(ExprPtr value)
        : value(std::move(value))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::WRITE << "\":";
        value->show(os);
        os << '}';
    }
};
