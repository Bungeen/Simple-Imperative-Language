#pragma once

#include <memory>

#include "istmt_node.h"
#include "inode_visitor.h"
#include "ilvalue_expr_node.h"

class ReadNode : public IStmtNode {
public:
    ReadNode(std::unique_ptr<ILvalueExprNode> variable)
        : variable_(std::move(variable))
    {}

    void Accept(INodeVisitor& visitor) const override {
        visitor.Visit(*this);
    }

    const ILvalueExprNode& GetTargetNode() const {
        return *variable_;
    }
private:
    std::unique_ptr<ILvalueExprNode> variable_;
};