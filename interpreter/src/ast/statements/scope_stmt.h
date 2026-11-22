#pragma once

#include <ostream>
#include <vector>
#include <utility>
#include <memory>

#include "stmt.h"
#include "json_format.h"

class ScopeStmt : public Stmt {
public:
    ScopeStmt(std::vector<StmtPtr> decls, StmtPtr body)
        : decls_(std::move(decls))
        , body_(std::move(body))
    {}

    const std::vector<StmtPtr>& get_decls() const {
        return decls_;
    }

    const Stmt& get_body() const {
        return *body_;
    }

    void execute(Interpreter& interpreter) const override {
        interpreter.Visit(*this);
    }

    void show(std::ostream& os) const override {
        os << "{\"" << json_format::SCOPE << "\":{";
        if (!decls_.empty()) {
            os << "\"" << json_format::DECLS << "\":[";
            for (std::size_t i = 0; i < decls_.size(); ++i) {
                if (i > 0) os << ",";
                decls_[i]->show(os);
            }
            os << "],";
        }
        os << "\"" << json_format::BODY << "\":";
        body_->show(os);
        os << "}}";
    }
private:
    std::vector<StmtPtr> decls_;
    StmtPtr body_;
};
