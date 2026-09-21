include_directories(ast interpreter io_providers value)

add_library(
    simple_interpreter_lib
    interpreter/interpreter.cpp
    interpreter/bin_op_executor.cpp
    io_providers/cli_input_provider.h
    io_providers/cli_output_provider.h
    io_providers/iinput_provider.h
    io_providers/ioutput_provider.h
    ast_reader/ast_reader.cpp
    ast/ast.h
    value/value.h
)

target_link_libraries(simple_interpreter_lib PUBLIC nlohmann_json::nlohmann_json)
target_include_directories(simple_interpreter_lib PUBLIC ast interpreter io_providers ast_reader value)