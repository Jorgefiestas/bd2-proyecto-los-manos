cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(bd2 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
include_directories(include)
set(SOURCES src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})


