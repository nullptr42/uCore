# Source files
FILE(GLOB KERNEL_SRC
    "${CMAKE_CURRENT_LIST_DIR}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/lib/libc/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/lib/log/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/lib/vt100/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/lib/version/*.c"
)
