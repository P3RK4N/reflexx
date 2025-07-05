# ----------------------------------------------
# Dependency Management Notice
#
# By default, this library does NOT automatically fetch or include any dependencies.
# 
# If you want to ensure that the default libraries are added and publicly linked,
# define the CMake variable `REFLEXX_ENABLE_JSON`, TODO:, to TRUE **before** including or building this library.
#
# Behavior:
# - If `REFLEXX_ENABLE_JSON` is set TRUE:
#     - This project will fetch and add `yyjson` if it is NOT already defined elsewhere.
#     - It will then link publicly to the library and enable related backend if exists.
#
# To enable:
#   cmake -DREFLEXX_ENABLE_JSON=ON ..
#   OR
#   set(REFLEXX_ENABLE_JSON ON) before adding this subdirectory
#
# Usage example inside your CMakeLists.txt:
#
#   if(ENABLE_YYJSON)
#     # Add or link yyjson target here as appropriate
#   endif()
#
# ----------------------------------------------

cmake_minimum_required(VERSION 3.20)
project(reflexx_project CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if (NOT COMMAND FetchContent_Declare)
    include(FetchContent)
endif()

if (NOT COMMAND CPMAddPackage)
    FetchContent_Declare(
        CPM
        GIT_REPOSITORY https://github.com/cpm-cmake/CPM.cmake.git
        GIT_TAG v0.40.8
    )
    FetchContent_MakeAvailable(CPM)
endif()

if (PROJECT_IS_TOP_LEVEL)
    set(REFLEXX_ENABLE_JSON ON)
endif()

set(REFLEXX_BACKEND_SOURCES "")
set(REFLEXX_BACKEND_LIBS "")

if (REFLEXX_ENABLE_JSON OR TARGET yyjson)
    if (NOT TARGET yyjson)
        CPMAddPackage(
            NAME yyjson
            GITHUB_REPOSITORY ibireme/yyjson
            GIT_TAG 0.11.1
        )
    endif()
    list(APPEND REFLEXX_BACKEND_LIBS yyjson)
    list(APPEND REFLEXX_BACKEND_SOURCES "${CMAKE_CURRENT_LIST_DIR}/include/reflexx/backends/yyjson_backend.hpp")
endif()

add_library(reflexx INTERFACE "${REFLEXX_BACKEND_SOURCES}")
target_link_libraries(reflexx INTERFACE "${REFLEXX_BACKEND_LIBS}")
target_include_directories(reflexx INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")
set_target_properties(
    reflexx 
    PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
)

if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(tst)
endif()