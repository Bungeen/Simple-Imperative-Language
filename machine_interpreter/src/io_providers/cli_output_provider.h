#pragma once

#include <iostream>

#include "output_provider.h"

class CliOutputProvider : public OutputProvider {
public:
    void write(std::string data) override {
        std::cout << data;
    }
};