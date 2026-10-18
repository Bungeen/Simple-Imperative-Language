add_library(compiler STATIC
    compiler.cpp
)

target_include_directories(compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(compiler PUBLIC operations nlohmann_json::nlohmann_json)
