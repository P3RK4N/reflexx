cmake_minimum_required(VERSION 3.20)
project(MyProject CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(reflexx INTERFACE)
target_include_directories(reflexx INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/deps")
include(FetchContent)

FetchContent_Declare(
    CPM
    GIT_REPOSITORY https://github.com/cpm-cmake/CPM.cmake.git
    GIT_TAG v0.40.8
)
FetchContent_MakeAvailable(CPM)

CPMAddPackage(
    NAME yyjson
    GITHUB_REPOSITORY ibireme/yyjson
    GIT_TAG 0.11.1
)

target_link_libraries(reflexx INTERFACE yyjson)

add_subdirectory(tst)