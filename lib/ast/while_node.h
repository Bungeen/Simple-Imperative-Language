#pragma once

#include <memory>

#include "iexpr_node.h"
#include "istmt_node.h"
#include "inode_visitor.h"

class WhileNode : public IStmtNode {
public:
    WhileNode(std::unique_ptr<IExprNode> condition, std::unique_ptr<IStmtNode> body)
        : condition_(std::move(condition))
        , body_(std::move(body))
    {}

    void Accept(INodeVisitor& visitor) const override {
        return visitor.Visit(*this);
    }

    const IExprNode& GetCondition() const {
        return *condition_;
    } 

    const IStmtNode& GetBody() const {
        return *body_;
    } 
private:
    std::unique_ptr<IExprNode> condition_;
    std::unique_ptr<IStmtNode> body_;
};