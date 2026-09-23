#pragma once

#include <string>

class OutputProvider {
public:
    virtual ~OutputProvider() = default;
    virtual void Write(std::string data) = 0;
};