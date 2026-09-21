#pragma once

#include "inode_visitor.h"
#include "value.h"
#include "iexpr_node.h"

class ILvalueExprNode : public IExprNode {
public:
    virtual Value& AcceptRef(INodeVisitor& visitor) const = 0;
};