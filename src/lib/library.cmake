
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/terminal/include
    ${CMAKE_CURRENT_LIST_DIR}/cxx/include
    ${CMAKE_CURRENT_LIST_DIR}/multiboot/include
)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/cxx)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/terminal/)

set(REINIX_LIBRARY_LIST
    rcxx
    terminal
)