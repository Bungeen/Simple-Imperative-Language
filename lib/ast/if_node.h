#pragma once

#include <memory>

#include "iexpr_node.h"
#include "istmt_node.h"
#include "inode_visitor.h"

class IfNode : public IStmtNode {
public:
    IfNode(std::unique_ptr<IExprNode> condition, 
           std::unique_ptr<IStmtNode> if_variant, 
           std::unique_ptr<IStmtNode> else_variant)
        : condition_(std::move(condition))
        , if_variant_(std::move(if_variant))
        , else_variant_(std::move(else_variant))
    {}

    void Accept(INodeVisitor& visitor) const override {
        return visitor.Visit(*this);
    }

    const IExprNode& GetCondition() const {
        return *condition_;
    } 

    const IStmtNode& GetIfVariant() const {
        return *if_variant_;
    } 

    const IStmtNode& GetElseVariant() const {
        return *else_variant_;
    } 
private:
    std::unique_ptr<IExprNode> condition_;
    std::unique_ptr<IStmtNode> if_variant_;
    std::unique_ptr<IStmtNode> else_variant_;
};