set (SOURCES
    emulator.cpp
)

set (HEADERS
    include/terminal/display.hpp
    include/terminal/emulator.hpp
)

add_library(terminal STATIC ${SOURCES} ${HEADERS})
