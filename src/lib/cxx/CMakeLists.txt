set(SOURCES
    printf.cpp
    snprintf.cpp
)

set(HEADERS
    allocator.hpp
    list.hpp
    stdio.hpp
)

add_library(rcxx STATIC ${SOURCES} ${HEADERS})