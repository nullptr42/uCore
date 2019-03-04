set(MSPHERE_LD_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/layout.ld)
set(MSPHERE_LD_FLAGS "-z max-page-size=4096")
set(MSPHERE_ARCH "x86_64")

set(PLATFORM_SRC
    platform/pc/boot.asm
    platform/pc/main.cpp
    platform/pc/console.cpp
)