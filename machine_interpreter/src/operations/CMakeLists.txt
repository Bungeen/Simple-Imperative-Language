add_library(operations INTERFACE bin_op.h command.h)
target_include_directories(operations INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})