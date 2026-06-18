cmake_minimum_required(VERSION 3.10)

project(MLSTP VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include)

set(GRAPH_SOURCES src/graph.cpp)

add_executable(MLSTP src/main.cpp ${GRAPH_SOURCES})
