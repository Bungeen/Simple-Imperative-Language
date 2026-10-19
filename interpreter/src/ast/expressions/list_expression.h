#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

#include "expression.h"
#include "json_format.h"
#include "value.h"


class ListExpr : public Expr {
public:
    ListExpr(std::vector<ExprPtr> params) 
        : params_(std::move(params))
    {}

    const std::vector<ExprPtr>& get_params() const  {
        return params_;
    }

    Value evaluate(Interpreter& interpreter) const override {
        return interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::ARRAY << "\": [";
        if (!params_.empty())
            params_[0]->show(os);

        for (std::size_t i = 1; i < params_.size(); ++i) {
            os << ",";
            params_[i]->show(os);
        }
        os << "]}";
    }
private:
    std::vector<ExprPtr> params_;
};