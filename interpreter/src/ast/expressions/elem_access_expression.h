#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "expression.h"
#include "lvalue_expression.h"
#include "json_format.h"


class ElemAccess : public LvalueExpr {
public:
    ElemAccess(ExprPtr list, ExprPtr selector) 
        : list_(std::move(list))
        , selector_(std::move(selector))
    {}

    Value evaluate(Interpreter& interpreter) const override {
        return interpreter.Visit(*this);
    }

    Value& evaluate_ref(Interpreter& interpreter) const override {
        return interpreter.VisitRef(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::ELEM << "\":\"";
        list_->show(os);
        os << ",\"" << json_format::INDEX << "\":";
        selector_->show(os);
        os << "\"}";
    }

    const Expr& get_list() const {
        return *list_;
    }

    const Expr& get_selector() const {
        return *selector_;
    }
private:
    ExprPtr list_;
    ExprPtr selector_;
};