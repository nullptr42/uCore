# Tell CMake that we cross-compile etc.
SET(CMAKE_SYSTEM_NAME      Generic)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)
SET(CMAKE_CROSSCOMPILING   1)

# Compiler binaries
SET(CMAKE_C_COMPILER   x86_64-elf-gcc)
SET(CMAKE_CXX_COMPILER x86_64-elf-g++)

# Assembler
SET(UCORE_ASM_LANG "ASM_NASM")
SET(CMAKE_ASM_NASM_OBJECT_FORMAT "elf64")

SET(COMPILE_FLAGS "-mno-red-zone -mcmodel=kernel")

# Source files
FILE(GLOB ISA_SRC
    "${CMAKE_CURRENT_LIST_DIR}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/*.asm"
)
