cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(bd2 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
include_directories(include)
set(SOURCES src/main.cpp)
set(INSERT_SOURCES webscripts/insert.cpp)
set(SEARCH_SOURCES webscripts/search.cpp)
set(RSEARCH_SOURCES webscripts/search-range.cpp)
set(READ_SOURCES webscripts/read.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
add_executable(insert ${INSERT_SOURCES})
add_executable(search ${SEARCH_SOURCES})
add_executable(range_search ${RSEARCH_SOURCES})
add_executable(read ${READ_SOURCES})


