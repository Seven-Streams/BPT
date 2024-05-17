cmake_minimum_required(VERSION 3.0.0)
project(code)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
enable_testing()
add_executable(code
BPT.cpp)