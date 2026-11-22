#pragma once

#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "stmt.h"
#include "json_format.h"

class FunDecl : public Stmt {
public:
    FunDecl(std::string name, std::vector<std::string> params, std::shared_ptr<Stmt> body)
        : name_(std::move(name))
        , params_(std::move(params))
        , body_(std::move(body))
    {}

    const std::string& get_name() const {
        return name_;
    }

    const std::vector<std::string>& get_params() const {
        return params_;
    }

    const Stmt& get_body() const {
        return *body_;
    }

    std::shared_ptr<Stmt> get_body_ptr() const {
        return body_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }

    void show(std::ostream& os) const override {
        os << "{\"" << json_format::FUN << "\":{\"" << json_format::NAME << "\":\"" << name_ << "\",\"";
        os << json_format::ARGS << "\":[";
        for (std::size_t i = 0; i < params_.size(); ++i) {
            if (i > 0) os << ",";
            os << "\"" << params_[i] << "\"";
        }
        os << "],\"" << json_format::BODY << "\":";
        body_->show(os);
        os << "}}";
    }
private:
    std::string name_;
    std::vector<std::string> params_;
    std::shared_ptr<Stmt> body_;
};
