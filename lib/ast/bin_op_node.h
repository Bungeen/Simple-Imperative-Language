#pragma once

#include <memory>

#include "iexpr_node.h"
#include "inode_visitor.h"

enum class BinOp {
    sum,
    sub,
    mult,
    div,
    remainder,
    greater,
    less,
    greater_eq,
    less_eq,
    equal,
    nequal,
    logical_and,
    logical_or
};

class BinOpNode : public IExprNode {
public:
    BinOpNode(std::unique_ptr<IExprNode> lhs, std::unique_ptr<IExprNode> rhs, BinOp operation)
        : lhs_(std::move(lhs))
        , rhs_(std::move(rhs))
        , operation_(operation)
    {}

    Value Accept(INodeVisitor& visitor) const override {
        return visitor.Visit(*this);
    }

    const IExprNode& GetLhs() const {
        return *lhs_;
    } 

    const IExprNode& GetRhs() const {
        return *rhs_;
    } 

    BinOp GetOperation() const {
        return operation_;
    }
private:
    std::unique_ptr<IExprNode> lhs_;
    std::unique_ptr<IExprNode> rhs_;
    BinOp operation_;
};