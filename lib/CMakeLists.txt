cmake_minimum_required(VERSION 3.3)

file(GLOB _gear_SOURCES httpclient/*.cpp)
file(GLOB _gear_HEADERS httpclient/*.hpp)

set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/../vendors)

add_definitions(-DASIO_STANDALONE -std=c++14 -stdlib=libc++)
include_directories(${VENDOR_DIR}/asio/asio/include ${VENDOR_DIR}/openssl/ios/include)

# For OSX build
add_library(_gear STATIC ${_gear_SOURCES} ${_gear_HEADERS})
target_link_libraries(_gear ${VENDOR_DIRS}/openssl/osx/lib/libcrypto.a ${VENDOR_DIRS}/openssl/osx/lib/libssl.a)

# For iOS build
add_library(_ios_gear STATIC ${_gear_SOURCES} ${_gear_HEADERS})
target_link_libraries(_ios_gear ${VENDOR_DIRS}/openssl/ios/lib/libcrypto.a ${VENDOR_DIRS}/openssl/ios/lib/libssl.a)

# For Android build
#add_library(_android_gear STATIC ${_gear_SOURCES} ${_gear_HEADERS})
#target_link_libraries(_android_gear ${VENDOR_DIRS}/openssl/ios/lib/libcrypto.a ${VENDOR_DIRS}/openssl/ios/lib/libssl.a)
