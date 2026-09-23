#pragma once

#include <iosfwd>

class Node {
public:
    virtual ~Node() = default;
    virtual void show(std::ostream& os) const = 0;
};