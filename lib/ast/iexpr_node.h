#pragma once

#include "inode_visitor.h"
#include "value.h"

class IExprNode {
public:
    virtual ~IExprNode() = default;
    virtual Value Accept(INodeVisitor& visitor) const = 0;
};