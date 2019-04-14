set(SOURCES
    printf.cpp
    snprintf.cpp
)

set(HEADERS
    allocator.hpp
    list.hpp
    stdio.hpp
)

add_library(rxx STATIC ${SOURCES} ${HEADERS})