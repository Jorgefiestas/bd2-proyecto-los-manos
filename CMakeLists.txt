cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(bd2 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(Threads)

include_directories(include)
set(SOURCES src/bptree.cpp src/pagemanager.cpp src/main.cpp)

add_executable(test_1 ${SOURCES})

