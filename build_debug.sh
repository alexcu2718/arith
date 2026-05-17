#!/usr/bin/env bash
set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

run_binary=false
run_tests=false
binary_args=()

while (($# > 0)); do
  case "$1" in
    --run)
      run_binary=true
      shift
      ;;
    --test)
      run_tests=true
      shift
      ;;
    --)
      shift
      binary_args=("$@")
      break
      ;;
    *)
      binary_args+=("$1")
      shift
      ;;
  esac
done

cmake --fresh -S "$ROOT_DIR" -B "$ROOT_DIR/build" -Dcpp-pointer_BUILD_TESTING=ON
cmake --build "$ROOT_DIR/build" -j"$(nproc)"

ln -sf "$ROOT_DIR/build/compile_commands.json" "$ROOT_DIR/compile_commands.json"

shopt -s nullglob
tidy_files=("$ROOT_DIR"/app/*.cpp)

if ((${#tidy_files[@]} > 0)); then
  clang-tidy -p "$ROOT_DIR/build" "${tidy_files[@]}"
fi

if [[ "$run_tests" == true ]]; then
  ctest --test-dir "$ROOT_DIR/build" --output-on-failure
fi

if [[ "$run_binary" == true ]]; then
  "$ROOT_DIR/build/app/cpp-pointer_app" "${binary_args[@]}"
fi