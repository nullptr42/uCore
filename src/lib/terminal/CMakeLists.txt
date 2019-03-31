set (SOURCES
    emulator.cpp
)

set (HEADERS
    display.hpp
    emulator.hpp
)

add_library(terminal STATIC ${SOURCES} ${HEADERS})
