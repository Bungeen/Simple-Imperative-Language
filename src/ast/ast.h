#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

struct Node {
    virtual ~Node();
    virtual void show(std::ostream& os) const = 0;
};

struct Expr : Node {};
struct Stmt : Node {};

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

enum class BinOp {
    Or,
    And,
    Equal,
    NotEqual,
    LessEqual,
    Less,
    GreaterEqual,
    Greater,
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
};

extern const BinOp ALL_BINOPS[13];

const std::string& spelling(BinOp op);

struct Const : Expr {
    const std::int64_t value;

    explicit Const(std::int64_t value);

    void show(std::ostream& os) const override;
};

struct Var : Expr {
    std::string name;

    explicit Var(std::string name);

    void show(std::ostream& os) const override;
};

struct BinOpExpr : Expr {
    BinOp op;
    ExprPtr lhs, rhs;

    BinOpExpr(BinOp op, ExprPtr lhs, ExprPtr rhs);

    void show(std::ostream& os) const override;
};

struct SeqStmt : Stmt {
    StmtPtr lhs, rhs;

    SeqStmt(StmtPtr lhs, StmtPtr rhs);

    void show(std::ostream& os) const override;
};

struct SkipStmt : Stmt {
    void show(std::ostream& os) const override;
};

struct WriteStmt : Stmt {
    ExprPtr value;

    explicit WriteStmt(ExprPtr value);

    void show(std::ostream& os) const override;
};

struct ReadStmt : Stmt {
    std::string name;

    explicit ReadStmt(std::string name);

    void show(std::ostream& os) const override;
};

struct AssignStmt : Stmt {
    std::string dst;
    ExprPtr src;

    AssignStmt(std::string dst, ExprPtr src);

    void show(std::ostream& os) const override;
};

struct CompoundAssignStmt : Stmt {
    std::string dst;
    BinOp op;
    ExprPtr src;

    CompoundAssignStmt(std::string dst, BinOp op, ExprPtr src);

    void show(std::ostream& os) const override;
};

struct WhileStmt : Stmt {
    ExprPtr cond;
    StmtPtr body;

    WhileStmt(ExprPtr cond, StmtPtr body);

    void show(std::ostream& os) const override;
};

struct DoWhileStmt : Stmt {
    StmtPtr body;
    ExprPtr cond;

    DoWhileStmt(StmtPtr body, ExprPtr cond);

    void show(std::ostream& os) const override;
};

struct ForStmt : Stmt {
    StmtPtr init;
    ExprPtr cond;
    StmtPtr step, body;

    ForStmt(StmtPtr init, ExprPtr cond, StmtPtr step, StmtPtr body);

    void show(std::ostream& os) const override;
};

struct IfStmt : Stmt {
    ExprPtr cond;
    StmtPtr thenBranch;
    StmtPtr elseBranch;

    IfStmt(ExprPtr cond, StmtPtr thenBranch, StmtPtr elseBranch = nullptr);

    void show(std::ostream& os) const override;
};


StmtPtr make_stmt_list(std::vector<StmtPtr>::iterator begin,
                       std::vector<StmtPtr>::iterator end);
