set(SOURCES
    printf.cpp
    snprintf.cpp
)

set(HEADERS
    include/allocator.hpp
    include/list.hpp
    include/stdio.hpp
)

add_library(rcxx STATIC ${SOURCES} ${HEADERS})