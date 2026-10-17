add_library(value STATIC value.cpp overload.h)
target_include_directories(value PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})