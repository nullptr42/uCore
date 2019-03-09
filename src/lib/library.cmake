
set(LIBRARY_SRC
    lib/terminal/emulator.cpp
    lib/cxx/snprintf.cpp
    lib/cxx/printf.cpp
)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/terminal/include
    ${CMAKE_CURRENT_LIST_DIR}/cxx/include
)