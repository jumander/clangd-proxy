cmake_minimum_required(VERSION 3.10)

project(clangd-proxy)

file(GLOB SOURCES "src/*.cpp")

add_executable(clangd-proxy ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/deps/json/include)

# Add option to export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)