set(REINIX_LD_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/layout.ld)
set(REINIX_LD_FLAGS "-z max-page-size=4096")
set(REINIX_ARCH "x86_64")

file(GLOB PLATFORM_SRC
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.asm"
)