function(reflexx_check_feature_compiles name source)
  set(options REQUIRED)
  set(oneValueArgs "")
  set(multiValueArgs "")
  cmake_parse_arguments(arg
      "${options}" "${oneValueArgs}" "${multiValueArgs}"
      ${ARGN}
  )

  set(test_bin_dir "${CMAKE_BINARY_DIR}/reflexx/try_compile/${name}")
  file(MAKE_DIRECTORY "${test_bin_dir}")

  set(test_file "${test_bin_dir}/${name}.cpp")
  file(WRITE "${test_file}" "${source}")

  message(STATUS "Checking feature ${name}...")

  try_compile(${name}_COMPILES
    ${test_bin_dir}
    SOURCES "${test_file}"
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
  )

  if(${name}_COMPILES)
    message(STATUS "Checking feature ${name}... -> TRUE")
  else()
    message(STATUS "Checking feature ${name}... -> FALSE")
    if(arg_REQUIRED)
      message(FATAL_ERROR "Feature ${name} is required but not available!")
    endif()
  endif()

  set(${name}_REFLEXX_COMPILES ${${name}_COMPILES} PARENT_SCOPE)
endfunction()
