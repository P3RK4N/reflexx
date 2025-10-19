if (NOT TARGET reflectcpp)
    CPMAddPackage(
        NAME reflectcpp
        GITHUB_REPOSITORY getml/reflect-cpp
        GIT_TAG v0.21.0
    )
endif()

if (NOT TARGET celero)
    CPMAddPackage(
        NAME celero
        GITHUB_REPOSITORY DigitalInBlue/Celero
        GIT_TAG v2.9.1
    )
endif() 

if (NOT TARGET glaze)
    CPMAddPackage(
        NAME glaze
        GITHUB_REPOSITORY stephenberry/glaze
        GIT_TAG v6.0.0
    )
endif()

add_executable(reflexx_benchmark benchmark.cpp data/citm_catalog.cpp data/profiles.cpp)

set_target_properties(
    reflexx_benchmark 
    PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(reflexx_benchmark PRIVATE reflexx reflectcpp celero glaze::glaze)
target_include_directories(reflexx_benchmark PRIVATE ${reflexx_project_SOURCE_DIR}/benchmark)
target_compile_definitions(reflexx_benchmark PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")