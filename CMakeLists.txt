cmake_minimum_required(VERSION 3.10)

project(GraphsConnectedComponents VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(GraphsConnectedComponents ${SOURCES})
