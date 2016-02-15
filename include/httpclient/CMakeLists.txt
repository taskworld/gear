
set(ASIO_SOURCES)
file(GLOB_RECURSE sources *.cpp *.hpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}\vendors\asio\asio\include)
link_directories(${CMAKE_SOURCE_DIR}\vendors\asio\asio\src)
