function(reflexx_add_compile_test)
  set(options REFLEXX_SHOULD_FAIL)
  set(oneValueArgs REFLEXX_NAME REFLEXX_SOURCE REFLEXX_TOOLCHAIN_FILE)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN})

  if(NOT ARG_REFLEXX_NAME)
    message(FATAL_ERROR "reflexx_run_compile_test requires NAME argument")
  endif()

  if(NOT ARG_REFLEXX_SOURCE)
    message(FATAL_ERROR "reflexx_run_compile_test requires SOURCE argument")
  endif()

  # Create source output directory
  set(reflexx_test_dir "${CMAKE_BINARY_DIR}/reflexx/try_compile_sources")
  file(MAKE_DIRECTORY "${reflexx_test_dir}")

  # Write source file
  set(source_file "${reflexx_test_dir}/${ARG_REFLEXX_NAME}.cpp")
  file(WRITE "${source_file}" "${ARG_REFLEXX_SOURCE}")

  # Create build dir for this specific test
  set(build_dir "${CMAKE_BINARY_DIR}/reflexx/try_compile_bin/")
  file(MAKE_DIRECTORY "${build_dir}")

  # Prepare arguments to CMake script
  set(cmd "${CMAKE_COMMAND}")
  list(APPEND cmd "-DREFLEXX_SOURCE_FILE=${source_file}")
  list(APPEND cmd "-DREFLEXX_SHOULD_FAIL=${ARG_REFLEXX_SHOULD_FAIL}")
  if(ARG_REFLEXX_TOOLCHAIN_FILE)
    list(APPEND cmd "-DCMAKE_TOOLCHAIN_FILE=${ARG_REFLEXX_TOOLCHAIN_FILE}")
  endif()
  list(APPEND cmd "-S ${PROJECT_SOURCE_DIR}/cmake/compile_test")
  list(APPEND cmd "-B ${build_dir}")

  # Test
  add_test(NAME "${ARG_REFLEXX_NAME}" COMMAND ${cmd})

endfunction()
