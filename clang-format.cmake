# Adapted from: https://arcanis.me/en/2015/10/17/cppcheck-and-clang-format
# TODO: Do not hardcode the clang-format binary path.

# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hpp)
#foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
#    string(FIND ${SOURCE_FILE} ${PROJECT_TRDPARTY_DIR} PROJECT_TRDPARTY_DIR_FOUND)
#    if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
#        list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
#    endif ()
#endforeach ()

add_custom_target(
        format
        COMMAND /usr/bin/clang-format
        -style=LLVM
        -i
        ${ALL_SOURCE_FILES}
)