#pragma once

#include <memory>

#include "istmt_node.h"
#include "inode_visitor.h"

class SeqNode : public IStmtNode {
public:
    SeqNode(std::unique_ptr<IStmtNode> lhs, std::unique_ptr<IStmtNode> rhs)
        : lhs_(std::move(lhs))
        , rhs_(std::move(rhs))
    {}

    void Accept(INodeVisitor& visitor) const override {
        visitor.Visit(*this);
    }

    const IStmtNode& GetLhs() const {
        return *lhs_;
    }

    const IStmtNode& GetRhs() const {
        return *rhs_;
    }
private:
    std::unique_ptr<IStmtNode> lhs_;
    std::unique_ptr<IStmtNode> rhs_;
};