# Toolchain for using a custom-built Clang/LLVM toolchain

# Path to your LLVM/Clang toolchain root
set(LLVM_ROOT "${CMAKE_CURRENT_LIST_DIR}/../bin/clang-p2996/install")

# Compilers
set(CMAKE_C_COMPILER "${LLVM_ROOT}/bin/clang")
set(CMAKE_CXX_COMPILER "${LLVM_ROOT}/bin/clang++")

# Linker
set(CMAKE_LINKER "${LLVM_ROOT}/bin/ld.lld")
set(CMAKE_AR "${LLVM_ROOT}/bin/llvm-ar")
set(CMAKE_NM "${LLVM_ROOT}/bin/llvm-nm")
set(CMAKE_RANLIB "${LLVM_ROOT}/bin/llvm-ranlib")
set(CMAKE_OBJDUMP "${LLVM_ROOT}/bin/llvm-objdump")
set(CMAKE_STRIP "${LLVM_ROOT}/bin/llvm-strip")

# System includes and libraries
# set(CMAKE_SYSROOT "${LLVM_ROOT}")
set(CMAKE_INCLUDE_PATH "${LLVM_ROOT}/include")
set(CMAKE_LIBRARY_PATH "${LLVM_ROOT}/lib;${LLVM_ROOT}/lib64")

# Include and link against custom libc++ and libc++abi
set(CMAKE_CXX_FLAGS_INIT "-nostdinc++ -isystem ${LLVM_ROOT}/include/c++/v1 -isystem ${LLVM_ROOT}/include/x86_64-unknown-linux-gnu/c++/v1 -freflection-latest")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-nostdlib++ -Wl,-rpath,${LLVM_ROOT}/lib/x86_64-unknown-linux-gnu -L${LLVM_ROOT}/lib/x86_64-unknown-linux-gnu -lc++experimental -lc++ -lc++abi")

# CMake modules and config packages
set(CMAKE_PREFIX_PATH "${LLVM_ROOT}")
set(LLVM_DIR "${LLVM_ROOT}/lib/cmake/llvm")
set(CLANG_DIR "${LLVM_ROOT}/lib/cmake/clang")

# Misc
set(CLANG_TIDY "${LLVM_ROOT}/bin/clang-tidy")
set(LLVM_COV "${LLVM_ROOT}/bin/llvm-cov")
set(LLVM_PROFDATA "${LLVM_ROOT}/bin/llvm-profdata")