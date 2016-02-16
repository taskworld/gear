cmake_minimum_required(VERSION 3.3)

set(SOURCE_FILES sample.cpp)
set(VENDOR_DIRS ${CMAKE_SOURCE_DIR}/vendors)

add_definitions(-DASIO_STANDALONE -std=c++14 -stdlib=libc++)
include_directories(${CMAKE_SOURCE_DIR}/gear ${VENDOR_DIRS}/websocketpp/ ${VENDOR_DIRS}/asio/asio/include ${VENDOR_DIRS}/openssl/lib/include)
add_subdirectory(gear)
add_executable(sample ${SOURCE_FILES})
target_link_libraries(sample _gear)

add_subdirectory(${VENDOR_DIRS}/gtest)
enable_testing()

file(GLOB TEST_SOURCE_FILES ${CMAKE_SOURCE_DIR}/test/*.cpp)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runTests ${TEST_SOURCE_FILES})
target_link_libraries(runTests gtest gtest_main _gear)
add_test(runTests runTests)
