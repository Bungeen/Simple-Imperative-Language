#pragma once

#include <string>

#include "ilvalue_expr_node.h"
#include "inode_visitor.h"

class VarNode : public ILvalueExprNode {
public:
    VarNode(const std::string& var_name)
        : var_name_(var_name)
    {}

    Value Accept(INodeVisitor& visitor) const override {
        return visitor.Visit(*this);
    }

    Value& AcceptRef(INodeVisitor& visitor) const override {
        return visitor.VisitLvalue(*this);
    }

    const std::string& GetVarName() const {
        return var_name_;
    } 
private:
    std::string var_name_;
};