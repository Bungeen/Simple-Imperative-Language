add_library(ast INTERFACE)
target_include_directories(ast INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/commons
    ${CMAKE_CURRENT_SOURCE_DIR}/expressions
    ${CMAKE_CURRENT_SOURCE_DIR}/statements)
