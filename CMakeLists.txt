cmake_minimum_required(VERSION 3.12)

project(
    simple_interpreter
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

include_directories(lib)
add_subdirectory(lib)
add_subdirectory(bin)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# enable_testing()
# add_subdirectory(tests)