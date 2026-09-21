add_library(ast STATIC ast.cpp)
target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})