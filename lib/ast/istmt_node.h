#pragma once

#include "inode_visitor.h"

class IStmtNode {
public:
    virtual ~IStmtNode() = default;
    virtual void Accept(INodeVisitor& visitor) const = 0;
};