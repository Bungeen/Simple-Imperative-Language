#pragma once

#include <string>

class OutputProvider {
public:
    virtual ~OutputProvider() = default;
    virtual void write(std::string data) = 0;
};