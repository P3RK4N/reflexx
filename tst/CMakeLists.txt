if (NOT TARGET Catch2::Catch2WithMain)
    CPMAddPackage(
        NAME catch2
        GITHUB_REPOSITORY catchorg/Catch2
        GIT_TAG v3.8.1
    )
endif()

include(CTest)
enable_testing()
include(reflexx_compile_test)

list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)

add_executable(test "playground.cpp" "util_test.cpp")
set_target_properties(test PROPERTIES EXPORT_COMPILE_COMMANDS OFF) # TODO: Remove when clangd gets fixed
target_link_libraries(test PRIVATE reflexx Catch2::Catch2WithMain)
catch_discover_tests(test)

reflexx_add_compile_test(
    REFLEXX_NAME dummy 
    REFLEXX_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}"
    REFLEXX_SOURCE "int main() {}" 
)