#pragma once

#include <memory>

#include "istmt_node.h"
#include "inode_visitor.h"
#include "iexpr_node.h"

class WriteNode : public IStmtNode {
public:
    WriteNode(std::unique_ptr<IExprNode> expr_node)
        : expr_node_(std::move(expr_node))
    {}

    void Accept(INodeVisitor& visitor) const override {
        visitor.Visit(*this);
    }

    const IExprNode& GetExprNode() const {
        return *expr_node_;
    }
private:
    std::unique_ptr<IExprNode> expr_node_;
};