cmake_minimum_required(VERSION 3.3)

file(GLOB _gear_SOURCES http/*.cpp socketio/*.cpp socketio/internal/*.cpp)
file(GLOB _gear_HEADERS http/*.hpp socketio/*.hpp socketio/*.h socketio/internal/*.h)

set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/../vendors)

add_definitions(-DASIO_STANDALONE -std=c++14 -stdlib=libc++)
include_directories(${VENDOR_DIR}/asio/asio/include ${VENDOR_DIR}/openssl/lib/include)

# For OSX build
add_library(_gear STATIC ${_gear_SOURCES} ${_gear_HEADERS})
target_link_libraries(_gear ${VENDOR_DIRS}/openssl/lib/lib/libcrypto.a ${VENDOR_DIRS}/openssl/lib/lib/libssl.a)
