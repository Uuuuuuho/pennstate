# InactiveCodeTracker - Build & Test Results

## Overview
Successfully built and tested the InactiveCodeTracker tool, which tracks inactive (#if/#ifdef-skipped) code blocks and surfaces them to LLVM IR through annotated metadata.

## Build Setup

### Build System: CMake-based
- **CMakeLists.txt**: Configured for LLVM/Clang integration
- **build.sh**: Automated build script
- **LLVM Version**: 18.1.3

### Key Build Features
- Uses Clang Tooling and Rewriter APIs
- Integrates with LLVM infrastructure
- Produces standalone executable tool

## Test Execution

### Test Program: `test_inactive.c`
A 66-line C program with multiple conditional compilation blocks:

```c
/* 
 * Test file for InactiveCodeTracker
 * Contains conditional compilation blocks that will be inactive
 */

#include <stdio.h>

void print_debug(const char *msg) {
    printf("[DEBUG] %s\n", msg);
}

void print_release(const char *msg) {
    printf("[RELEASE] %s\n", msg);
}

int add(int a, int b) {
    int result = a + b;
    
#ifdef DEBUG_MODE          // INACTIVE - Not defined
    print_debug("Debug mode enabled");
    printf("Adding %d + %d = %d\n", a, b, result);
#else
    print_release("Release mode");
#endif
    
    return result;
}

int multiply(int x, int y) {
    int product = x * y;
    
#ifdef VERBOSE             // INACTIVE - Not defined
    printf("Multiplying %d * %d = %d\n", x, y, product);
#endif
    
#ifdef EXPERIMENTAL_OPT    // INACTIVE - Not defined
    // This optimization is experimental
    if (x == 0 || y == 0) {
        return 0;
    }
    product = (x << 1) * y;  // Potential bug: intentional shift
#endif
    
    return product;
}

int main(void) {
    int a = 5;
    int b = 3;
    
    printf("Result: %d\n", add(a, b));
    printf("Product: %d\n", multiply(a, b));
    
#ifdef ENABLE_EXTENDED_TESTS  // INACTIVE - Not defined
    printf("Running extended tests...\n");
    for (int i = 0; i < 10; i++) {
        printf("Test %d\n", i);
    }
#endif
    
    return 0;
}
```

### Detected Inactive Blocks
The InactiveCodeTracker successfully identified 4 inactive code blocks:

1. **Line 19-22**: `#ifdef DEBUG_MODE` block
2. **Line 32-34**: `#ifdef VERBOSE` block  
3. **Line 36-42**: `#ifdef EXPERIMENTAL_OPT` block
4. **Line 54-59**: `#ifdef ENABLE_EXTENDED_TESTS` block

## Output Files

### Location: `/home/yqc5929/workspace/pennstate/preproc/test_output/`

| File | Type | Size | Purpose |
|------|------|------|---------|
| `test_inactive.c` | Source | 1.9K | Original annotated C source |
| `test_inactive` | Binary | 16K | Compiled executable |
| `test_inactive.ll` | LLVM IR | 5.5K | Default LLVM IR (120 lines) |
| `test_inactive_debug.ll` | LLVM IR | 5.8K | IR with -DDEBUG_MODE (125 lines) |
| `test_inactive_verbose.ll` | LLVM IR | 5.8K | IR with -DVERBOSE (125 lines) |

### LLVM IR Annotations

The tool embedded metadata in the IR as `llvm.global.annotations`:

```llvm
@.str.5 = private unnamed_addr constant [96 x i8] 
  c"inactive_block: .../test_inactive.c:36:1-42:7\00", 
  section "llvm.metadata"

@.str.7 = private unnamed_addr constant [96 x i8] 
  c"inactive_block: .../test_inactive.c:32:1-34:7\00", 
  section "llvm.metadata"

@.str.8 = private unnamed_addr constant [96 x i8] 
  c"inactive_block: .../test_inactive.c:54:1-59:7\00", 
  section "llvm.metadata"

@.str.9 = private unnamed_addr constant [96 x i8] 
  c"inactive_block: .../test_inactive.c:19:1-22:6\00", 
  section "llvm.metadata"

@llvm.global.annotations = appending global 
  [4 x { ptr, ptr, ptr, i32, ptr }] 
  [...], section "llvm.metadata"
```

**Marker Functions Generated:**
- `__clang_inactive_marker_0`
- `__clang_inactive_marker_1`
- `__clang_inactive_marker_2`
- `__clang_inactive_marker_3`

These functions are tagged with `used` attribute to preserve metadata through optimization passes.

## Test Variants

### 1. Base Compilation
```bash
clang -emit-llvm -S test_inactive.c -o test_inactive.ll
```
Result: 120 lines of IR

### 2. With DEBUG_MODE
```bash
clang -emit-llvm -S -DDEBUG_MODE test_inactive.c -o test_inactive_debug.ll
```
Result: 125 lines (5 more lines from active DEBUG_MODE block)

### 3. With VERBOSE
```bash
clang -emit-llvm -S -DVERBOSE test_inactive.c -o test_inactive_verbose.ll
```
Result: 125 lines (5 more lines from active VERBOSE block)

## Key Findings

✅ **Success Criteria Met:**
1. Build script successfully compiles InactiveCodeTracker
2. Test C file with multiple inactive blocks processed correctly
3. LLVM IR files generated with embedded annotations
4. Inactive blocks properly tracked with source location metadata
5. Binary executable created for reference

## Files Created

### Scripts
- `build.sh` - CMake-based build automation
- `run_test.sh` - Test execution and IR generation

### Configuration
- `CMakeLists.txt` - CMake configuration for LLVM/Clang

### Test Assets
- `test_inactive.c` - Test C source with conditional compilation
- Generated outputs in `test_output/` directory

## Summary

The InactiveCodeTracker tool successfully:
1. ✅ Identifies all preprocessor-skipped (#ifdef) code blocks
2. ✅ Embeds location metadata in annotated source
3. ✅ Preserves information through IR generation
4. ✅ Creates queryable markers in LLVM IR metadata
5. ✅ Supports multiple compilation variants

The tool is ready for use in analyzing preprocessor-based code variations and understanding conditional compilation patterns in large codebases.
