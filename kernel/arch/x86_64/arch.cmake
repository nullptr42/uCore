# Tell CMake that we cross-compile etc.
SET(CMAKE_SYSTEM_NAME      Generic)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)
SET(CMAKE_CROSSCOMPILING   1)

# Compiler binaries
SET(CMAKE_C_COMPILER   x86_64-elf-gcc)
SET(CMAKE_CXX_COMPILER x86_64-elf-g++)

# Assembler
SET(MSPHERE_ASM_LANG "ASM_NASM")
SET(CMAKE_ASM_NASM_OBJECT_FORMAT "elf64")
SET(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -i ${CMAKE_SOURCE_DIR}/")

SET(COMPILE_FLAGS "-mno-red-zone -mcmodel=kernel -mno-sse")

# Source files
FILE(GLOB ISA_SRC
    "${CMAKE_CURRENT_LIST_DIR}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/*.asm"
    "${CMAKE_CURRENT_LIST_DIR}/apic/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/apic/*.asm"
    "${CMAKE_CURRENT_LIST_DIR}/interrupt/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/interrupt/*.asm"
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/include/)
