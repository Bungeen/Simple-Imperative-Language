cmake_minimum_required(VERSION 3.14)
project(my_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        TLS_VERIFY TRUE
    )
FetchContent_MakeAvailable(nlohmann_json)

add_subdirectory(src)
add_subdirectory(bin)