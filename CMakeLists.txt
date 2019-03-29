cmake_minimum_required(VERSION 2.8)

include("clang-format.cmake")
include(ExternalProject)

ExternalProject_Add(
    reinix
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src
    INSTALL_COMMAND ""
)