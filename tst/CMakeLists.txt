if (NOT TARGET Catch2::Catch2WithMain)
    CPMAddPackage(
        NAME catch2
        GITHUB_REPOSITORY catchorg/Catch2
        GIT_TAG v3.8.1
    )
endif()

add_executable(test "playground.cpp" "util_test.cpp")
set_target_properties(test PROPERTIES EXPORT_COMPILE_COMMANDS OFF) # TODO: Remove when clangd gets fixed
target_link_libraries(test PRIVATE reflexx Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(CTest)
include(Catch)
catch_discover_tests(test)
