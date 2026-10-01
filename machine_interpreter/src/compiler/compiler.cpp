#include "compiler.h"
#include "parser.h"

std::vector<Command> compile(std::string_view source) {
    return Parser(source).program();
}
