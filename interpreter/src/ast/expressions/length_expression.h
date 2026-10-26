#pragma once

#include <cstdint>
#include <ostream>

#include "expression.h"
#include "json_format.h"
#include "value.h"


class Length : public Expr {
public:
    Length(ExprPtr list) 
        : list_(std::move(list))
    {}

    const Expr& get_list() const  {
        return *list_;
    }

    Value evaluate(Interpreter& interpreter) const override {
        return interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::LENGTH << "\":";
        list_->show(os);
        os << '}';
    }
private:
    ExprPtr list_;
};