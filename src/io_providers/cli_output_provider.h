#pragma once

#include <iostream>

#include "output_provider.h"

class CliOutputProvider : public OutputProvider {
public:
    void Write(std::string data) override {
        std::cout << data;
    }
};