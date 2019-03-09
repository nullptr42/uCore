# Set cross-compile information
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_CROSSCOMPILING 1)

# Set compiler binaries
set(CMAKE_C_COMPILER x86_64-elf-gcc)
set(CMAKE_CXX_COMPILER x86_64-elf-g++)

# Setup NASM
set(REINIX_ASM_LANG "ASM_NASM")
set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf64")
set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -I ${CMAKE_SOURCE_DIR}/")

set(COMPILE_FLAGS "-mno-red-zone -mcmodel=kernel -mno-sse")

file(GLOB ARCH_SRC
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/asm/*.asm"
)

include_directories(arch/x86_64/include)
