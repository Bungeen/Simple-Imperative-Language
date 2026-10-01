add_library(compiler STATIC
    compiler.cpp
    lexer.cpp
    parser.cpp
    compiler_error.cpp
)

target_include_directories(compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(compiler PUBLIC operations)
