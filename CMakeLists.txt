cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(bd2 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(Threads)

include_directories(./include)
set(SOURCES src/bptree/bptree.cpp src/bptree/pagemanager.cpp src/random_file/random_file.cpp
	src/hash/dynamic_hash.cpp src/main.cpp)

add_executable(test_1 ${SOURCES})

