#pragma once

#include <ostream>
#include <utility>

#include "stmt.h"
#include "expression.h"
#include "pattern.h"
#include "json_format.h"


class SwitchStmt : public Stmt {
public:
    SwitchStmt(ExprPtr value, std::vector<std::pair<PatternPtr, StmtPtr>> cases)
        : value_(std::move(value))
        , cases_(std::move(cases))
    {}


    void show(std::ostream& os) const {
        os << "{\"" << json_format::SWITCH << "\":{\"" << json_format::SCRUTINEE << "\":";
        value_->show(os);
        os << ",\"" << json_format::BRANCHES << "\": [";
        if (!cases_.empty()) {
            show_branch(os, 0);
        }

        for (std::size_t i = 1; i < cases_.size(); ++i) {
            os << ",";
            show_branch(os, i);
        }
        os << "]}";
    }

    const Expr& get_value() const {
        return *value_;
    } 

    const std::vector<std::pair<PatternPtr, StmtPtr>>& get_cases() const {
        return cases_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }
private:
    void show_branch(std::ostream& os, std::size_t pos) const {
        os << "{";
        cases_[pos].first->show(os);
        os << ", \"" << json_format::BRANCH << "\": ";
        cases_[pos].second->show(os);
        os << "}";
    }

    ExprPtr value_;
    std::vector<std::pair<PatternPtr, StmtPtr>> cases_;
};