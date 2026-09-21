#pragma once

#include "stmt.h"
#include "json_format.h"

#include <ostream>
#include <string>
#include <utility>


class ReadStmt : public Stmt {
private:
    std::string name;
public:
    ReadStmt(std::string name)
        : name(std::move(name))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::READ << "\":\"" << name << "\"}";
    }
};
