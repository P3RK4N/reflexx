add_executable(reflexx_benchmark simple_benchmark.cpp)

set_target_properties(
    reflexx_benchmark 
    PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(reflexx_benchmark PRIVATE reflexx)

foreach(json_file IN ITEMS "5mb.json" "citm_catalog.json")
    add_custom_command(TARGET reflexx_benchmark POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_LIST_DIR}/${json_file}"
            $<TARGET_FILE_DIR:reflexx_benchmark>/${json_file}
    )
endforeach()