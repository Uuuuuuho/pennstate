#!/bin/bash
PLUGIN_PATH="./build/lib/libHelloWorld.so"
LL_DIR="/linux-6.10/all_llvm_ir"

for ll in "$LL_DIR"/*.ll; do
  echo "===== [ $(basename "$ll") ] ====="
  opt -load-pass-plugin "$PLUGIN_PATH" -passes=hello-world -disable-output "$ll" 2>&1
  echo
done
