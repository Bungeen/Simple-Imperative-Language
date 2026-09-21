#pragma once

#include <memory>

#include "istmt_node.h"
#include "inode_visitor.h"
#include "ilvalue_expr_node.h"

class AssignNode : public IStmtNode {
public:
    AssignNode(std::unique_ptr<ILvalueExprNode> lhs, std::unique_ptr<IExprNode> rhs)
        : lhs_(std::move(lhs))
        , rhs_(std::move(rhs))
    {}

    void Accept(INodeVisitor& visitor) const override {
        visitor.Visit(*this);
    }

    const ILvalueExprNode& GetLhs() const {
        return *lhs_;
    }

    const IExprNode& GetRhs() const {
        return *rhs_;
    }
private:
    std::unique_ptr<ILvalueExprNode> lhs_;
    std::unique_ptr<IExprNode> rhs_;
};
