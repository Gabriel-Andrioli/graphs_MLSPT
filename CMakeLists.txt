cmake_minimum_required(VERSION 3.10)

project(MLSTP VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include)

set(GRAPH_SOURCES 
    src/graph.cpp 
    src/random_manager.cpp 
    src/solver/common.cpp 
    src/solver/greedy.cpp 
    src/solver/randomized.cpp 
    src/solver/reactive.cpp
)

add_executable(MLSTP src/main.cpp ${GRAPH_SOURCES})
add_executable(generator src/generator.cpp src/random_manager.cpp)

