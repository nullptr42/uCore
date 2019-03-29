cmake_minimum_required(VERSION 2.8)

# Disable useless "compile tests"
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Project options
set(REINIX_PLATFORM "pc" CACHE STRING "Platform")
set(REINIX_OPT_LEVEL "2" CACHE STRING "Optimization Level (0, 1, 2, 3, fast, s, ...)")
include("src/platform/${REINIX_PLATFORM}/platform.cmake")
include("src/arch/${REINIX_ARCH}/architecture.cmake")

project(reinix-kernel C CXX)

include("clang-format.cmake")

# Target specific ASM language (e.g. ASM_NASM for i386 or x86_64)
if(REINIX_ASM_LANG)
    enable_language(${REINIX_ASM_LANG})
endif()

set(CMAKE_USE_RELATIVE_PATHS ON)

# Set compiler flags
set(COMPILE_FLAGS    "${COMPILE_FLAGS} -ffreestanding -O${REINIX_OPT_LEVEL} -Wall -Wextra -Wno-pointer-arith -Wno-switch")
set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS}   ${COMPILE_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS} -std=c++17 -fno-rtti -fno-exceptions")

# Set linker flags
set(LINK_FLAGS "-T ${REINIX_LD_SCRIPT} -ffreestanding -O${REINIX_OPT_LEVEL} -nostdlib -lgcc ${REINIX_LD_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")

include("src/kernel/kernel.cmake")
include("src/lib/library.cmake")

include_directories(src/include)
add_executable(reinix ${KERNEL_SRC} ${LIBRARY_SRC} ${ARCH_SRC} ${PLATFORM_SRC})