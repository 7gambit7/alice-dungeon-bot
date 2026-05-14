#!/bin/bash
# Test runner. Compiles main.cpp, then runs every tests/*.in.
# - If a matching .expected exists, output is diffed against it.
# - Otherwise a smoke check: must be either a one-line parser-rejection,
#   or a normal trace whose last line starts with "result ".
set -u

cd "$(dirname "$0")/.."
ROOT="$PWD"
TESTS="$ROOT/tests"

# --- locate or build the binary ---
# If TASK_BIN is set (e.g. by CMake/CTest), use it as-is. Otherwise compile.
if [ -n "${TASK_BIN:-}" ]; then
    echo "[build] skipped — using TASK_BIN=$TASK_BIN"
    TASK="$TASK_BIN"
else
    echo "[build] g++ -std=c++17 -O2 src/*.cpp -o task"
    if ! g++ -std=c++17 -O2 -Wall -Wextra src/*.cpp -o task; then
        echo "BUILD FAILED" >&2
        exit 1
    fi
    TASK="$ROOT/task"
fi

# --- generate stress test on demand ---
if [ ! -f "$TESTS/31_stress_chain.in" ]; then
    bash "$TESTS/gen_stress.sh" "$TESTS/31_stress_chain.in"
fi

pass=0
fail=0
failed=()

for in_file in "$TESTS"/*.in; do
    name=$(basename "$in_file" .in)
    expected="$TESTS/${name}.expected"

    "$TASK" "$in_file"

    if [ -f "$expected" ]; then
        if diff -u "$expected" "$ROOT/result.txt" > /dev/null 2>&1; then
            printf "PASS  %s\n" "$name"
            pass=$((pass + 1))
        else
            printf "FAIL  %s\n" "$name"
            diff -u "$expected" "$ROOT/result.txt" | sed 's/^/        /' | head -40
            failed+=("$name")
            fail=$((fail + 1))
        fi
    else
        # smoke check: either 1-line parser rejection or trace ending with "result ..."
        lines=$(wc -l < "$ROOT/result.txt")
        last=$(tail -1 "$ROOT/result.txt")
        if [[ "$last" == result\ * ]]; then
            printf "PASS  %s (smoke, %d lines, %s)\n" "$name" "$lines" "$last"
            pass=$((pass + 1))
        elif [[ "$lines" == "1" ]]; then
            printf "PASS  %s (smoke, parser-rejection: %s)\n" "$name" "$last"
            pass=$((pass + 1))
        else
            printf "FAIL  %s (smoke, no result line, last: %s)\n" "$name" "$last"
            failed+=("$name")
            fail=$((fail + 1))
        fi
    fi
done

echo ""
echo "Total: PASS=$pass  FAIL=$fail"
if [ "$fail" -ne 0 ]; then
    echo "Failed: ${failed[*]}"
    exit 1
fi