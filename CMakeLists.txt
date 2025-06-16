cmake_minimum_required(VERSION 3.20)
project(MyProject CXX)

# Optional: Use C++17 or later
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLOB all .cpp and .c files from src/
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

# Add the executable
add_executable(main ${SOURCES})