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

add_executable(reflexx_runtime_tests
    "playground.cpp"
    "complex.cpp"
    "runtime/util/class_util_test.cpp"
    "runtime/util/enum_conv_test.cpp"
    "runtime/util/non_serializable_category_type_test.cpp"
    "runtime/util/typename_chain_test.cpp"
)
set_target_properties(
    reflexx_runtime_tests 
    PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(reflexx_runtime_tests PRIVATE reflexx Catch2::Catch2WithMain)
catch_discover_tests(reflexx_runtime_tests)

set(REFLEXX_COMPILE_TESTS
    tst/compile/ok_basic_struct.cpp
    tst/compile/ok_nested_struct.cpp
    tst/compile/fail_raw_pointer.cpp
    tst/compile/fail_reference_member.cpp
    tst/compile/ok_enum_integral.cpp
    tst/compile/fail_union.cpp
    tst/compile/fail_function_ptr.cpp
    tst/compile/ok_enum_string.cpp
    tst/compile/ok_vector.cpp
    tst/compile/fail_not_default_constructible_deserialize.cpp
    tst/compile/ok_ignore_raw_ptr_with_relaxed.cpp
    tst/compile/ok_builtin_string.cpp
    tst/compile/ok_builtin_vector.cpp
)

foreach(file IN LISTS REFLEXX_COMPILE_TESTS)
    get_filename_component(test_name ${file} NAME_WE)

    # Check if filename starts with "fail_"
    string(FIND ${test_name} "fail_" fail_pos)
    if(fail_pos EQUAL 0)
        reflexx_add_compile_test(
            SHOULD_FAIL
            SOURCE_FILE "${reflexx_project_SOURCE_DIR}/${file}"
            TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}"
            INCLUDE_DIRS "${reflexx_project_SOURCE_DIR}/include"
        )
    else()
        reflexx_add_compile_test(
            SOURCE_FILE "${reflexx_project_SOURCE_DIR}/${file}"
            TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}"
            INCLUDE_DIRS "${reflexx_project_SOURCE_DIR}/include"
        )
    endif()
endforeach()