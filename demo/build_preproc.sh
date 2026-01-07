#!/bin/bash

set COMPILER=clang
set -e
rm *.i # Clean up old files

if [ "$COMPILER" = "clang" ]; then
    COMPILER_CMD=clang
else
    COMPILER_CMD=gcc
fi

# Input source doesn't include any control-flow macros
$COMPILER_CMD -E -DENABLE_FEATURE demo_preproc_simple.c -o demo_preproc_simple.i

# Show the last 60 lines of the preprocessed output
# Input source includes control-flow based on DEBUG_MODE macro
$COMPILER_CMD -E -DDEBUG_MODE -DENABLE_FEATURE demo_preproc.c -o demo_preproc_debug.i
tail -60 demo_preproc_debug.i
$COMPILER_CMD -E demo_preproc.c -o demo_preproc_release.i
tail -60 demo_preproc_release.i
