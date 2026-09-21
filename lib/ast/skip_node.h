#pragma once

#include "istmt_node.h"
#include "inode_visitor.h"

class SkipNode : public IStmtNode {
public:
    void Accept(INodeVisitor& visitor) const override {
        visitor.Visit(*this);
    }
};