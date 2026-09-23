add_library(interpreter STATIC interpreter.cpp interpreter.h bin_op_executor.cpp)
target_include_directories(interpreter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(interpreter PUBLIC operations io_providers)