
set(LIBRARY_SRC
    src/lib/terminal/emulator.cpp
    src/lib/cxx/snprintf.cpp
    src/lib/cxx/printf.cpp
)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/terminal/include
    ${CMAKE_CURRENT_LIST_DIR}/cxx/include
    ${CMAKE_CURRENT_LIST_DIR}/multiboot/include
)