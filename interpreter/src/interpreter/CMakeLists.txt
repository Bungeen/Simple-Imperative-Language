add_library(interpreter STATIC interpreter.cpp bin_op_executor.cpp interpreter.h)
target_include_directories(interpreter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(interpreter PUBLIC ast io_providers)