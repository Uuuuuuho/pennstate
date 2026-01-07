#!/bin/bash

# build AddNotePass.so
# clang++ -fPIC -shared ../AddNotePass.cpp -o libAddNotePass.so \
#   $(llvm-config --cxxflags --ldflags --system-libs --libs all)

# generate LLVM IR with the pass
clang -O1 -emit-llvm -S test.c -o test.ll
clang -fpass-plugin=./libAddNotePass.so -fno-discard-value-names -O1 -emit-llvm -S test.c -o test_with_note.ll