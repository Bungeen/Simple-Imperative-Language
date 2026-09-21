#pragma once

#include <string>

class IOutputProvider {
public:
    virtual ~IOutputProvider() = default;
    virtual void Write(std::string data) = 0;
};