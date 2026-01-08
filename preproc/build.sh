#!/bin/bash

# Build script for InactiveCodeTracker clang tool using CMake
set -e

BUILD_DIR="build"

echo "[*] Checking CMake installation..."
if ! command -v cmake &> /dev/null; then
    echo "ERROR: cmake not found. Please install CMake."
    exit 1
fi

echo "[*] Checking LLVM installation..."
if ! command -v llvm-config &> /dev/null; then
    echo "ERROR: llvm-config not found. Please install LLVM development tools."
    exit 1
fi

LLVM_VERSION=$(llvm-config --version)
echo "[*] Found LLVM version: $LLVM_VERSION"

# Create and enter build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "[*] Generating build files with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

echo "[*] Building InactiveCodeTracker..."
cmake --build . --config Release

echo "[+] Build successful!"
echo "[*] Binary location: $(pwd)/bin/InactiveCodeTracker"
