cmake_minimum_required(VERSION 3.11)

project(clangd-proxy)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.hpp")

add_executable(clangd-proxy ${HEADERS} ${SOURCES})

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(clangd-proxy PRIVATE nlohmann_json::nlohmann_json)

# Add option to export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)