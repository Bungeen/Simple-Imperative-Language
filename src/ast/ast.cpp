#include "ast.h"
#include "json_format.h"

#include <ostream>
#include <stdexcept>
#include <utility>

Node::~Node() = default;

const BinOp ALL_BINOPS[] = {
    BinOp::Or, BinOp::And,
    BinOp::Equal, BinOp::NotEqual,
    BinOp::LessEqual, BinOp::Less,
    BinOp::GreaterEqual, BinOp::Greater,
    BinOp::Add, BinOp::Subtract,
    BinOp::Multiply, BinOp::Divide, BinOp::Modulo
};

const std::string& spelling(BinOp op) {
    switch (op) {
        case BinOp::Or:           return binop_text::OR;
        case BinOp::And:          return binop_text::AND;
        case BinOp::Equal:        return binop_text::EQUAL;
        case BinOp::NotEqual:     return binop_text::NOT_EQUAL;
        case BinOp::LessEqual:    return binop_text::LESS_EQUAL;
        case BinOp::Less:         return binop_text::LESS;
        case BinOp::GreaterEqual: return binop_text::GREATER_EQUAL;
        case BinOp::Greater:      return binop_text::GREATER;
        case BinOp::Add:          return binop_text::ADD;
        case BinOp::Subtract:     return binop_text::SUBTRACT;
        case BinOp::Multiply:     return binop_text::MULTIPLY;
        case BinOp::Divide:       return binop_text::DIVIDE;
        case BinOp::Modulo:       return binop_text::MODULO;
    }
    throw std::invalid_argument("unknown binary operator");
}

void Const::show(std::ostream& os) const {
    os << "{\"" << json_format::CONST << "\":" << value << '}';
}

Const::Const(std::int64_t value) : value(value) {}

void Var::show(std::ostream& os) const {
    os << "{\"" << json_format::VAR << "\":\"" << name << "\"}";
}

Var::Var(std::string name) : name(std::move(name)) {}

void BinOpExpr::show(std::ostream& os) const {
    os << "{\"" << json_format::BINOP << "\":\"" << spelling(op) << "\",\"" << json_format::LEFT << "\":";
    lhs->show(os);
    os << ",\"" << json_format::RIGHT << "\":";
    rhs->show(os);
    os << '}';
}

BinOpExpr::BinOpExpr(BinOp op, ExprPtr lhs, ExprPtr rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

void SeqStmt::show(std::ostream& os) const {
    os << "{\"" << json_format::SEQ << "\":{\"" << json_format::LEFT << "\":";
    lhs->show(os);
    os << ",\"" << json_format::RIGHT << "\":";
    rhs->show(os);
    os << "}}";
}

SeqStmt::SeqStmt(StmtPtr lhs, StmtPtr rhs)
        : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

void SkipStmt::show(std::ostream& os) const {
    os << "{\"" << json_format::SKIP << "\":null}";
}

void WriteStmt::show(std::ostream& os) const {
    os << "{\"" << json_format::WRITE << "\":";
    value->show(os);
    os << '}';
}

WriteStmt::WriteStmt(ExprPtr value) : value(std::move(value)) {}

void ReadStmt::show(std::ostream& os) const {
    os << "{\"" << json_format::READ << "\":\"" << name << "\"}";
}

ReadStmt::ReadStmt(std::string name) : name(std::move(name)) {}

void AssignStmt::show(std::ostream& os) const {
    os << "{\"" << json_format::ASSN << "\":{\"" << json_format::DST << "\":\"" << dst << "\",\""
       << json_format::SRC << "\":";
    src->show(os);
    os << "}}";
}

AssignStmt::AssignStmt(std::string dst, ExprPtr src)
        : dst(std::move(dst)), src(std::move(src)) {}

void CompoundAssignStmt::show(std::ostream& os) const {
    os << "{\"" << json_format::COMPOUND_ASSN << "\":{\"" << json_format::DST << "\":\"" << dst
       << "\",\"" << json_format::BINOP << "\":\"" << spelling(op) << "\",\"" << json_format::SRC
       << "\":";
    src->show(os);
    os << "}}";
}

CompoundAssignStmt::CompoundAssignStmt(std::string dst, BinOp op, ExprPtr src)
        : dst(std::move(dst)), op(op), src(std::move(src)) {}

void WhileStmt::show(std::ostream& os) const {
    os << "{\"" << json_format::WHILE << "\":{\"" << json_format::COND << "\":";
    cond->show(os);
    os << ",\"" << json_format::BODY << "\":";
    body->show(os);
    os << "}}";
}

WhileStmt::WhileStmt(ExprPtr cond, StmtPtr body)
        : cond(std::move(cond)), body(std::move(body)) {}

void DoWhileStmt::show(std::ostream& os) const {
    os << "{\"" << json_format::DO_WHILE << "\":{\"" << json_format::BODY << "\":";
    body->show(os);
    os << ",\"" << json_format::COND << "\":";
    cond->show(os);
    os << "}}";
}

DoWhileStmt::DoWhileStmt(StmtPtr body, ExprPtr cond)
        : body(std::move(body)), cond(std::move(cond)) {}

void ForStmt::show(std::ostream& os) const {
    os << "{\"" << json_format::FOR << "\":{\"" << json_format::INIT << "\":";
    init->show(os);
    os << ",\"" << json_format::COND << "\":";
    cond->show(os);
    os << ",\"" << json_format::STEP << "\":";
    step->show(os);
    os << ",\"" << json_format::BODY << "\":";
    body->show(os);
    os << "}}";
}

ForStmt::ForStmt(StmtPtr init, ExprPtr cond, StmtPtr step, StmtPtr body)
        : init(std::move(init)), cond(std::move(cond)),
          step(std::move(step)), body(std::move(body)) {}

void IfStmt::show(std::ostream& os) const {
    os << "{\"" << json_format::IF << "\":{\"" << json_format::COND << "\":";
    cond->show(os);
    os << ",\"" << json_format::THEN << "\":";
    thenBranch->show(os);
    if (elseBranch) {
        os << ",\"" << json_format::ELSE << "\":";
        elseBranch->show(os);
    }
    os << "}}";
}

IfStmt::IfStmt(ExprPtr cond, StmtPtr thenBranch, StmtPtr elseBranch)
        : cond(std::move(cond)), thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

StmtPtr make_stmt_list(std::vector<StmtPtr>::iterator begin,
                       std::vector<StmtPtr>::iterator end) {
    if (begin == end) {
        throw std::invalid_argument("empty statement list");
    }

    StmtPtr res = std::move(*--end);
    while (end != begin) {
        --end;
        res = std::make_unique<SeqStmt>(std::move(*end), std::move(res));
    }
    return res;
}
