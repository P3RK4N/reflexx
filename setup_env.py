import os
import sys
import platform
import subprocess
import tarfile
import shutil
from pathlib import Path

# Platform check
if platform.system() != "Linux" or platform.machine() != "x86_64":
    sys.exit("This script only works on x86_64 Linux.")

# Arg check
if len(sys.argv) != 2 or sys.argv[1] not in {"compile_from_source", "use_prebuilt_clang"}:
    sys.exit("Usage: python script.py [compile_from_source | use_prebuilt_clang]")

REPO = "P3RK4N/reflexx"
TAG = "v0.1"
FILENAME = "clang-p2996-x86_64-unknown-linux-gnu.tar.gz"
SRC_DIR = Path("bin/clang-p2996/src")
BIN_DIR = Path("bin/clang-p2996/bin")
INSTALL_DIR = Path("bin/clang-p2996/install")

if SRC_DIR.exists():
    shutil.rmtree(SRC_DIR)

INSTALL_DIR.mkdir(parents=True, exist_ok=True)
SRC_DIR.mkdir(parents=True, exist_ok=True)
BIN_DIR.mkdir(parents=True, exist_ok=True)

def download_with_gh():
    print("Downloading release asset using GitHub CLI...")
    subprocess.run([
        "gh", "release", "download", TAG,
        "--repo", REPO,
        "--pattern", FILENAME,
        "--dir", INSTALL_DIR
    ], check=True)
    print("Download complete.")

def extract_tar_gz(archive_path: Path, extract_to: Path):
    print(f"Extracting {archive_path} to {extract_to}/...")
    with tarfile.open(archive_path, "r:gz") as tar:
        tar.extractall(path=extract_to)
    print("Extraction complete.")


if sys.argv[1] == "use_prebuilt_clang":
    archive_path = INSTALL_DIR / FILENAME
    download_with_gh()
    extract_tar_gz(archive_path, INSTALL_DIR)
    archive_path.unlink()
    print(f"Deleted archive: {archive_path}")
    
elif sys.argv[1] == "compile_from_source":
    print("See you in few hours...")
    print("Cloning LLVM project...")

    subprocess.run([
        "git", "clone", "--depth=1", "--branch", "p2996",
        "https://github.com/bloomberg/clang-p2996", str(SRC_DIR)
    ], check=True)

    print("Configuring CMake...")
    subprocess.run([
        "cmake", "-G", "Ninja",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_CXX_STANDARD=17",
        "-DCMAKE_C_COMPILER=clang",
        "-DCMAKE_CXX_COMPILER=clang++",
        "-DCMAKE_INSTALL_PREFIX=" + str(INSTALL_DIR),
        "-B", BIN_DIR,
        "-S", str(SRC_DIR / "llvm"),

        '-DLLVM_ENABLE_PROJECTS=clang;clang-tools-extra;llvm;lld',
        '-DLLVM_ENABLE_RUNTIMES=libcxx;libcxxabi;libunwind;compiler-rt',
        "-DLLVM_ENABLE_ASSERTIONS=OFF",
        "-DLIBCXXXABI_USE_LLVM_UNWINDER=ON",
        "-DLLVM_TARGETS_TO_BUILD=X86"

    ], check=True)

    print("Building...")
    subprocess.run([
        "cmake", "--build", BIN_DIR, "--parallel"
    ], check=True)

    print("Installing...")
    subprocess.run([
        "cmake", "--install", BIN_DIR
    ], check=True)

    print("Build and install complete.")