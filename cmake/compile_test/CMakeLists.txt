cmake_minimum_required(VERSION 3.15)

project(ReflexxTryCompileTest LANGUAGES C CXX)

if(NOT DEFINED REFLEXX_SOURCE_FILE)
  message(FATAL_ERROR "REFLEXX_SOURCE_FILE must be defined")
endif()

if(NOT DEFINED REFLEXX_SHOULD_FAIL)
  message(FATAL_ERROR "REFLEXX_SHOULD_FAIL must be defined")
endif()

try_compile(REFLEXX_COMPILE_TEST_RESULT
  "${CMAKE_BINARY_DIR}"
  SOURCES "${REFLEXX_SOURCE_FILE}"
  CXX_STANDARD 26
  CXX_STANDARD_REQUIRED ON
)

message(STATUS "ANTE")

if(REFLEXX_SHOULD_FAIL)
  if(REFLEXX_COMPILE_TEST_RESULT)
    message(FATAL_ERROR "Expected compilation to fail, but it succeeded")
  endif()
else()
  if(NOT REFLEXX_COMPILE_TEST_RESULT)
    message(FATAL_ERROR "Expected compilation to succeed, but it failed")
  endif()
endif()
