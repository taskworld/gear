file(GLOB lib_SOURCES httpclient/*.cpp)
file(GLOB lib_HEADERS httpclient/*.hpp)

add_library(LIB ${lib_SOURCES} ${lib_HEADERS})
