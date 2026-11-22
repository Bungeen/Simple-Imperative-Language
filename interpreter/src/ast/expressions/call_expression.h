#pragma once

#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "expression.h"
#include "json_format.h"

class CallExpr : public Expr {
public:
    CallExpr(std::string name, std::vector<ExprPtr> args)
        : name_(std::move(name))
        , args_(std::move(args))
    {}

    const std::string& get_name() const {
        return name_;
    }

    const std::vector<ExprPtr>& get_args() const {
        return args_;
    }

    Value evaluate(Interpreter& interpreter) const override {
        return interpreter.Visit(*this);
    }

    void show(std::ostream& os) const override {
        os << "{\"" << json_format::CALL << "\":{\"" << json_format::NAME << "\":\"" << name_ << "\",\"";
        os << json_format::ARGS << "\":[";
        for (std::size_t i = 0; i < args_.size(); ++i) {
            if (i > 0) os << ",";
            args_[i]->show(os);
        }
        os << "]}}";
    }
private:
    std::string name_;
    std::vector<ExprPtr> args_;
};
