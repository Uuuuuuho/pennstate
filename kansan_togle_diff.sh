#!/bin/bash

set -e
# Docker Path
KERNEL_DIR=/workspace/linux
# Local Path
# KERNEL_DIR=~/workspace/linux

# Use LLVM toolchain (Clang, lld, llvm-ar, etc.)
# Set to empty string to use GCC
# LLVM_FLAG="LLVM=1"
LLVM_FLAG=""

cd "$KERNEL_DIR"

echo "[1] Cleaning kernel..."
make $LLVM_FLAG mrproper

echo "[2] Creating default config..."
make $LLVM_FLAG defconfig

echo "[3] Enabling CONFIG_KASAN..."
scripts/config --enable CONFIG_KASAN
make $LLVM_FLAG olddefconfig
make $LLVM_FLAG prepare
make $LLVM_FLAG drivers/usb/storage/alauda.i
cp drivers/usb/storage/alauda.i printk_yes.i

echo "[4] Disabling CONFIG_KASAN..."
scripts/config --disable CONFIG_KASAN
make $LLVM_FLAG olddefconfig
make $LLVM_FLAG prepare
make $LLVM_FLAG drivers/usb/storage/alauda.i
cp drivers/usb/storage/alauda.i printk_no.i

echo "[5] Diffing results..."
diff -u printk_no.i printk_yes.i | less

