#pragma once

#include "iexpr_node.h"
#include "inode_visitor.h"

class ConstNode : public IExprNode {
public:
    ConstNode(Value value) 
        : value_(value)
    {}

    Value Accept(INodeVisitor& visitor) const override {
        return visitor.Visit(*this);
    }

    const Value& GetValue() const  {
        return value_;
    } 
private:
    Value value_;
};