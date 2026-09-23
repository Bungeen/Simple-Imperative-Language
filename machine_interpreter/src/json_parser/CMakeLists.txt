add_library(json_parser STATIC program_json.cpp)
target_include_directories(json_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(json_parser PUBLIC nlohmann_json::nlohmann_json operations)