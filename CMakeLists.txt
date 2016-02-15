cmake_minimum_required(VERSION 3.3)
project(GEAR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES sample.cpp)
set(VENDOR_DIRS ${CMAKE_SOURCE_DIR}/vendors)

include_directories(${VENDOR_DIRS}/openssl/osx/include)

add_definitions(-DASIO_STANDALONE)
include_directories(${CMAKE_SOURCE_DIR}/lib/httpclient ${VENDOR_DIRS}/asio/asio/include)

add_subdirectory(lib)

add_executable(GEAR ${SOURCE_FILES})
target_link_libraries(GEAR LIB ${VENDOR_DIRS}/openssl/osx/lib/libcrypto.a ${VENDOR_DIRS}/openssl/osx/lib/libssl.a)
