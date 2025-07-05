file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/reflexx/try_compile_bin/")

function(reflexx_add_compile_test)
  set(options SHOULD_FAIL)
  set(oneValueArgs NAME SOURCE_FILE TOOLCHAIN_FILE)
  set(multiValueArgs INCLUDE_DIRS)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_SOURCE_FILE)
    message(FATAL_ERROR "reflexx_add_compile_test requires ARG_SOURCE_FILE argument")
  endif()

  # Set default name from SOURCE_FILE if ARG_NAME is not provided
  if(NOT ARG_NAME)
    get_filename_component(ARG_NAME "${ARG_SOURCE_FILE}" NAME_WE)
  endif()

  # Prepare arguments to CMake script
  set(cmd "${CMAKE_COMMAND}")
  list(APPEND cmd "-DREFLEXX_SOURCE_FILE=${ARG_SOURCE_FILE}")
  list(APPEND cmd "-DREFLEXX_SHOULD_FAIL=${ARG_SHOULD_FAIL}")
  if(ARG_TOOLCHAIN_FILE)
    list(APPEND cmd "-DCMAKE_TOOLCHAIN_FILE=${ARG_TOOLCHAIN_FILE}")
  endif()
  if(ARG_INCLUDE_DIRS)
    string(REPLACE ";" "\\;" ARG_INCLUDE_DIRS_ESCAPED "${ARG_INCLUDE_DIRS}")
    list(APPEND cmd "-DREFLEXX_INCLUDE_DIRS=${ARG_INCLUDE_DIRS_ESCAPED}")
  endif()
  list(APPEND cmd "-S ${reflexx_project_SOURCE_DIR}/cmake/compile_test")
  list(APPEND cmd "${CMAKE_BINARY_DIR}/reflexx/try_compile_bin/")

  # Test
  add_test(NAME "${ARG_NAME}" COMMAND ${cmd})

endfunction()
