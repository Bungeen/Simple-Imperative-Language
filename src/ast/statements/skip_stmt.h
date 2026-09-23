#pragma once

#include <ostream>

#include "stmt.h"
#include "json_format.h"


class SkipStmt : public Stmt {
public:
    void show(std::ostream& os) const {
        os << "{\"" << json_format::SKIP << "\":null}";
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }
};
