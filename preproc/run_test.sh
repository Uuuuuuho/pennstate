#!/bin/bash

# Test execution script for InactiveCodeTracker
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
TRACKER="$BUILD_DIR/bin/InactiveCodeTracker"
TEST_SOURCE="$SCRIPT_DIR/test_inactive.c"
OUTPUT_DIR="$SCRIPT_DIR/test_output"

echo "[*] InactiveCodeTracker Test Suite"
echo "=================================="

# Check if tracker was built
if [ ! -f "$TRACKER" ]; then
    echo "[ERROR] InactiveCodeTracker not found at $TRACKER"
    echo "[*] Running build script first..."
    bash "$SCRIPT_DIR/build.sh" || exit 1
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

echo "[*] Test file: $TEST_SOURCE"
echo "[*] Output directory: $OUTPUT_DIR"

# Copy test file to output directory
cp "$TEST_SOURCE" "$OUTPUT_DIR/test_inactive.c"

echo ""
echo "[*] Step 1: Running InactiveCodeTracker on test_inactive.c..."
cd "$OUTPUT_DIR"

# Run InactiveCodeTracker with the test file
# The tool will add annotations to the source file
if "$TRACKER" "test_inactive.c" -- ; then
    echo "[+] InactiveCodeTracker completed successfully"
else
    EXIT_CODE=$?
    echo "[!] InactiveCodeTracker exited with code $EXIT_CODE (this might be OK)"
fi

echo ""
echo "[*] Step 2: Generating LLVM IR from annotated source..."

# Generate LLVM IR from the modified source
OUTPUT_LL="test_inactive.ll"
if clang -emit-llvm -S test_inactive.c -o "$OUTPUT_LL" 2>&1; then
    echo "[+] LLVM IR generated: $OUTPUT_LL"
    echo "[*] File size: $(wc -l < "$OUTPUT_LL") lines"
else
    echo "[!] Warning: clang IR generation had issues"
fi

echo ""
echo "[*] Step 3: Generating standard C binary for reference..."

# Compile to executable for reference
OUTPUT_EXE="test_inactive"
if clang test_inactive.c -o "$OUTPUT_EXE" 2>&1; then
    echo "[+] Binary compiled: $OUTPUT_EXE"
else
    echo "[!] Binary compilation had issues"
fi

echo ""
echo "[*] Step 4: Testing with different preprocessor flags..."

# Test with DEBUG_MODE enabled
TEST_DEBUG_LL="test_inactive_debug.ll"
clang -emit-llvm -S -DDEBUG_MODE test_inactive.c -o "$TEST_DEBUG_LL" 2>&1
echo "[+] Debug variant IR: $TEST_DEBUG_LL"

# Test with VERBOSE flag
TEST_VERBOSE_LL="test_inactive_verbose.ll"
clang -emit-llvm -S -DVERBOSE test_inactive.c -o "$TEST_VERBOSE_LL" 2>&1
echo "[+] Verbose variant IR: $TEST_VERBOSE_LL"

echo ""
echo "=================================="
echo "[+] Test execution complete!"
echo ""
echo "Output files:"
ls -lh "$OUTPUT_DIR"/*.{ll,c} 2>/dev/null || echo "Files generated"
echo ""
echo "Modified source file contents:"
head -20 test_inactive.c
echo "..."
echo ""
echo "LLVM IR Statistics:"
echo "  Base IR lines: $(wc -l < "$OUTPUT_LL")"
echo "  Debug IR lines: $(wc -l < "$TEST_DEBUG_LL")"
echo "  Verbose IR lines: $(wc -l < "$TEST_VERBOSE_LL")"
echo ""
echo "To view the LLVM IR file: cat $OUTPUT_LL"
echo "To view annotations in IR: grep -n 'annotate' $OUTPUT_LL"
