SET(MSPHERE_LD_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/layout.ld)
SET(MSPHERE_LD_FLAGS "-z max-page-size=4096")

OPTION(BOOT_GX "Graphical boot" OFF)

IF(BOOT_GX)
    SET(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -dBOOT_GX")
ENDIF()

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
    "${CMAKE_CURRENT_LIST_DIR}/pm/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/pm/*.asm"
    "${CMAKE_CURRENT_LIST_DIR}/vm/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/vm/*.asm"
)
