SET(UCORE_LD_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/layout.ld)
SET(UCORE_LD_FLAGS "-z max-page-size=4096")

# Source files
FILE(GLOB PLATFORM_SRC
    "${CMAKE_CURRENT_LIST_DIR}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/*.asm"
    "${CMAKE_CURRENT_LIST_DIR}/interrupt/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/interrupt/*.asm"
    "${CMAKE_CURRENT_LIST_DIR}/multiboot/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/multiboot/*.asm"
    "${CMAKE_CURRENT_LIST_DIR}/apic/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/apic/*.asm"
)
