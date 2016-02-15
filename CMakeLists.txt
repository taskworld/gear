cmake_minimum_required(VERSION 3.3)
project(GEAR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES sample.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include/httpclient)

add_executable(GEAR ${SOURCE_FILES})
