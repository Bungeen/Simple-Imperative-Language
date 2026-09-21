#pragma once

#include <iostream>

#include "ioutput_provider.h"

class CliOutputProvider : public IOutputProvider {
public:
    void Write(std::string data) override {
        std::cout << data;
    }
};