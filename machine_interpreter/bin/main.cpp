#include <iostream>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "compiler.h"
#include "interpreter.h"
#include "cli_input_provider.h"
#include "cli_output_provider.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source-file>" << std::endl;
        return 1;
    }

    try {
        std::ifstream input(argv[1]);

        if (!input) {
            throw std::runtime_error("Cannot open input file");
        }

        const std::string source{std::istreambuf_iterator<char>(input),
                                 std::istreambuf_iterator<char>()};

        auto program = compile(source);

        Interpreter interpreter(std::make_unique<CliInputProvider>(),
                                std::make_unique<CliOutputProvider>());

        interpreter.interpret(std::move(program));
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
