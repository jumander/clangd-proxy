cmake_minimum_required(VERSION 3.10)

project(clangd-proxy)

file(GLOB SOURCES "src/*.cpp")

add_executable(clangd-proxy ${SOURCES})
