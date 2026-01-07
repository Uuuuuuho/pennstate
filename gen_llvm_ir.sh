#!/bin/bash

OUT_DIR="all_llvm_ir"
LOG_DIR="failed_logs"
mkdir -p "$OUT_DIR" "$LOG_DIR"

LINUX_DIR="/workspace/linux"
TARGET_DIR=$LINUX_DIR
TARGET_DIR="."

find $TARGET_DIR -name '*.o.cmd' | while read -r cmdfile; do
  # print which cmdfile is being processed
  # echo "[*] Processing $cmdfile"
  # savedcmd=... 한 라인만 추출
  cmdline=$(grep -oP 'savedcmd_[^:]+: *=.*' "$cmdfile" | sed 's/savedcmd_[^:]*: *= //')
  [[ -n "$cmdline" ]] || continue

  # objtool 등 뒤에 붙은 명령 제거
  clang_cmd=$(echo "$cmdline" | sed -E 's/;.*$//')

  # -c 패킹 대상 추출
  cfiles=$(echo "$clang_cmd" | grep -oP '[^ ]+\.c(?=($| ))')
  [[ -f "$cfiles" ]] || continue

  # 출력 파일 이름 정의
  llfile="$OUT_DIR/${cfiles//\//_}.ll"
  [[ -f "$llfile" ]] && continue

  # -c -> -emit-llvm -S 로 변환
  clang_cmd=$(echo "$clang_cmd" | sed 's/ -c / -emit-llvm -S /')

  # 기존 -o 제거
  clang_cmd=$(echo "$clang_cmd" | sed -E 's/ -o [^ ]+//g')

  # 새 -o 추가
  clang_cmd="$clang_cmd -o $llfile"

  # 실행
  echo "[*] $cfiles → $llfile"
  if ! eval "$clang_cmd" >/dev/null 2> "$LOG_DIR/$(basename "$llfile").log"; then
    echo "    ↳ Failed (see $LOG_DIR/$(basename "$llfile").log)"
  fi
done

