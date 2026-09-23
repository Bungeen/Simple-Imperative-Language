#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "stmt.h"
#include "json_format.h"


class ReadStmt : public Stmt {
public:
    ReadStmt(std::string name)
        : variable_name_(std::move(name))
    {}

    void show(std::ostream& os) const {
        os << "{\"" << json_format::READ << "\":\"" << variable_name_ << "\"}";
    }

    const std::string& get_variable_name() const {
        return variable_name_;
    }
private:
    std::string variable_name_;
};
