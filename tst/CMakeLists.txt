
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")

add_executable(test ${SOURCES})
target_compile_options(test PUBLIC -save-temps -fverbose-asm)
target_link_libraries(test PUBLIC reflexx)
