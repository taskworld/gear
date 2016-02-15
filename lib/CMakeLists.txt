file(GLOB _gear_SOURCES httpclient/*.cpp)
file(GLOB _gear_HEADERS httpclient/*.hpp)

add_library(_gear ${gear_SOURCES} ${gear_HEADERS})
