#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

#include "expression.h"
#include "json_format.h"
#include "value.h"


class SexpExpr : public Expr {
public:
    SexpExpr(std::string tag, std::vector<ExprPtr> params) 
        : tag_(std::move(tag))
        , params_(std::move(params))
    {}

    const std::vector<ExprPtr>& get_params() const  {
        return params_;
    }

    const std::string& get_tag() const  {
        return tag_;
    }

    Value evaluate(Interpreter& interpreter) const override {
        return interpreter.Visit(*this);
    }

    void show(std::ostream& os) const {
        os << "{\"" << json_format::SEXP << "\": \"" << tag_ << "\", \"" << json_format::ARGS << "\": [";
        if (!params_.empty())
            params_[0]->show(os);

        for (std::size_t i = 1; i < params_.size(); ++i) {
            os << ",";
            params_[i]->show(os);
        }
        os << "]}";
    }
private:
    std::string tag_;
    std::vector<ExprPtr> params_;
};