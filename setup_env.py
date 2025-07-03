import os
import subprocess
import tarfile
from pathlib import Path

REPO = "P3RK4N/reflexx"
TAG = "v0.1"
FILENAME = "clang-p2996-x86_64-unknown-linux-gnu.tar.gz"
OUT_FILENAME = "clang-p2996-x86_64-unknown-linux-gnu"
DEST_DIR = Path("deps")

def download_with_gh():
    print("Downloading release asset using GitHub CLI...")
    DEST_DIR.mkdir(parents=True, exist_ok=True)
    subprocess.run([
        "gh", "release", "download", TAG,
        "--pattern", FILENAME,
        "--dir", str(DEST_DIR)
    ], check=True)
    print("Download complete.")

def extract_tar_gz(archive_path: Path, extract_to: Path):
    print(f"Extracting {archive_path} to {extract_to}/...")
    with tarfile.open(archive_path, "r:gz") as tar:
        tar.extractall(path=extract_to)
    print("Extraction complete.")

def main():
    archive_path = DEST_DIR / FILENAME

    download_with_gh()
    extract_tar_gz(archive_path, DEST_DIR / OUT_FILENAME)
    archive_path.unlink()  # delete archive
    print(f"Deleted archive: {archive_path}")

if __name__ == "__main__":
    main()
