if (NOT TARGET Catch2::Catch2WithMain)
    CPMAddPackage(
        NAME catch2
        GITHUB_REPOSITORY catchorg/Catch2
        GIT_TAG v3.8.1
    )
endif()

if(NOT COMMAND add_test)
    include(CTest)
endif()
enable_testing()
include(reflexx_compile_test)
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)


############################################################
##################### RUNTIME TESTS ########################
############################################################

add_executable(reflexx_runtime_tests
    "runtime/playground.cpp"
    "runtime/complex.cpp"
    "runtime/backends_test.cpp"
    "runtime/type_handler_list_test.cpp"
    "runtime/util/class_util_test.cpp"
    "runtime/util/enum_conv_test.cpp"
    "runtime/util/non_serializable_category_type_test.cpp"
    "runtime/util/serializable_number_test.cpp"
    "runtime/util/move_construct_if_possible_test.cpp"
    "runtime/util/std_util_test.cpp"
    "runtime/ptr_graph_handler_test.cpp"
)
set_target_properties(
    reflexx_runtime_tests 
    PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(reflexx_runtime_tests PRIVATE reflexx Catch2::Catch2WithMain)
catch_discover_tests(reflexx_runtime_tests)


############################################################
#################### COMPTIME TESTS ########################
############################################################


file(GLOB_RECURSE REFLEXX_COMPILE_TESTS "${reflexx_project_SOURCE_DIR}/tst/comptime/*.cpp")

foreach(source_file IN LISTS REFLEXX_COMPILE_TESTS)
    get_filename_component(test_name ${source_file} NAME_WE)

    # Check if filename starts with "fail_"
    string(FIND ${test_name} "fail_" fail_pos)
    set(should_fail "")
    if(fail_pos EQUAL 0)
        set(should_fail SHOULD_FAIL)
    endif()

    reflexx_add_compile_test(
        ${should_fail}
        SOURCE_FILE "${source_file}"
        TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}"
        INCLUDE_DIRS "${reflexx_project_SOURCE_DIR}/include"
    )
endforeach()

############################################################
###################### BENCHMARK ###########################
############################################################

add_executable(reflexx_benchmark
    "benchmark/simple_benchmark.cpp"
)
set_target_properties(
    reflexx_benchmark 
    PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(reflexx_benchmark PRIVATE reflexx)