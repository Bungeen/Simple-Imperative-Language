#include <iostream>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "ast.h"
#include "ast_reader.h"
#include "cli_input_provider.h"
#include "cli_output_provider.h"

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "filename required" << std::endl;

        return 0;
    }
        
    std::ifstream fin(argv[1]);
    
    if (!fin.is_open()) {
        std::cout << "Didn't managed to open specified file";

        return 0;
    }

    nlohmann::json program{};

    fin >> program;

    std::unique_ptr<IStmtNode> base_node;

    try {
        base_node = AstReader().ReadAst(program);
    } catch (std::exception exception) {
        std::cout << "Invalid json file" << std::endl;
    }

    Interpreter interpreter(std::make_unique<CliInputProvider>(), 
                            std::make_unique<CliOutputProvider>());

    try {
        base_node->Accept(interpreter);
    } catch(std::exception exception) {
        std::cout << exception.what() << std::endl;
    }

    return 0;
}