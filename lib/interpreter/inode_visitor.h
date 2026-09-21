#pragma once

#include "value.h"

class ConstNode;
class VarNode;
class BinOpNode;
class SkipNode;
class SeqNode;
class AssignNode;
class ReadNode;
class WriteNode;
class WhileNode;
class DoWhileNode;
class IfNode;

class INodeVisitor {
public:
    virtual Value& VisitLvalue(const VarNode& node) = 0;

    virtual Value Visit(const VarNode& node) = 0;
    virtual Value Visit(const ConstNode& node) = 0;
    virtual Value Visit(const BinOpNode& node) = 0;

    virtual void Visit(const SkipNode& node) = 0;
    virtual void Visit(const SeqNode& node) = 0;
    virtual void Visit(const AssignNode& node) = 0;
    virtual void Visit(const ReadNode& node) = 0;
    virtual void Visit(const WriteNode& node) = 0;
    virtual void Visit(const WhileNode& node) = 0;
    virtual void Visit(const DoWhileNode& node) = 0;
    virtual void Visit(const IfNode& node) = 0;
};