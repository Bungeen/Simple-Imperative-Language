#pragma once

#include "expression.h"
#include "json_format.h"

#include <ostream>
#include <string>
#include <utility>


class Var : public Expr {
public:
    std::string name;

    Var(std::string name) 
        : name(std::move(name))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::VAR << "\":\"" << name << "\"}";
    }
};
