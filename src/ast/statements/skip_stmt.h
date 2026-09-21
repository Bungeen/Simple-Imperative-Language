#pragma once

#include "stmt.h"
#include "json_format.h"

#include <ostream>


class SkipStmt : public Stmt {
    void show(std::ostream& os) const {
        os << "{\"" << json_format::SKIP << "\":null}";
    }
};
