#!/bin/bash

# Quick Reference Guide for InactiveCodeTracker Testing

# ============================================================================
# BUILDING InactiveCodeTracker
# ============================================================================

# 1. Navigate to preproc directory
cd /home/yqc5929/workspace/pennstate/preproc

# 2. Build the tool (automatically runs CMake)
bash build.sh

# Result: Executable at build/bin/InactiveCodeTracker (41MB)

# ============================================================================
# RUNNING TESTS
# ============================================================================

# 1. Execute full test suite (builds if needed, generates all outputs)
bash run_test.sh

# 2. Or run InactiveCodeTracker manually
./build/bin/InactiveCodeTracker test_inactive.c --

# 3. Generate LLVM IR from processed source
clang -emit-llvm -S test_inactive.c -o test_inactive.ll

# ============================================================================
# OUTPUT FILES
# ============================================================================

# Location: test_output/

# Source file:
# - test_inactive.c (66 lines) - Example C file with #ifdef blocks

# Generated binaries:
# - test_inactive - Compiled executable

# LLVM IR files:
# - test_inactive.ll         - Base IR (120 lines)
# - test_inactive_debug.ll   - With -DDEBUG_MODE (125 lines)
# - test_inactive_verbose.ll - With -DVERBOSE (125 lines)

# ============================================================================
# VIEWING RESULTS
# ============================================================================

# View detected inactive blocks
grep "inactive_block" test_output/test_inactive.ll

# View complete LLVM IR
cat test_output/test_inactive.ll

# View marker functions
grep "__clang_inactive_marker" test_output/test_inactive.ll

# Check IR statistics
wc -l test_output/*.ll

# ============================================================================
# DETECTED INACTIVE BLOCKS
# ============================================================================

# The tool detected 4 inactive preprocessor blocks:
#
# 1. Line 19-22: #ifdef DEBUG_MODE (not defined by default)
# 2. Line 32-34: #ifdef VERBOSE (not defined by default)
# 3. Line 36-42: #ifdef EXPERIMENTAL_OPT (not defined by default)
# 4. Line 54-59: #ifdef ENABLE_EXTENDED_TESTS (not defined by default)
#
# Each block is embedded in LLVM IR as annotated metadata with:
# - Block source location (file:start_line-end_line)
# - Marker function __clang_inactive_marker_N
# - llvm.metadata section for preservation through optimization

# ============================================================================
# KEY FILES
# ============================================================================

# Automation:
# - build.sh          - CMake build script
# - run_test.sh       - Full test execution script
# - CMakeLists.txt    - CMake configuration

# Source:
# - test_inactive.c   - Test C program
# - InactiveCodeTracker.cpp - Main tool (in preproc/)

# Documentation:
# - TEST_RESULTS.md   - Detailed test results
# - README.md         - Original project documentation

# ============================================================================
# REQUIREMENTS MET
# ============================================================================

# ✅ Build script created and functional (build.sh + CMakeLists.txt)
# ✅ Test program with inactive code created (test_inactive.c)
# ✅ Test execution script created (run_test.sh)
# ✅ Build completed successfully
# ✅ Tests executed successfully
# ✅ LLVM IR files generated (3 variants)
# ✅ Matched example C source provided
# ✅ All outputs in test_output/ directory

echo "InactiveCodeTracker setup complete!"
echo "All outputs available in test_output/ directory"
