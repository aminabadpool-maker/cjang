#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

TMP_BIN="$(mktemp -d "${TMPDIR:-/tmp}/cjang_tg_suite.XXXXXX")"
trap 'rm -rf "$TMP_BIN"' EXIT

shopt -s nullglob
test_files=(tests/*tour*guard*.c tests/*guard*tour*.c)
shopt -u nullglob

if [ "${#test_files[@]}" -eq 0 ]; then
    echo "[ERR] No cjang_guard / cjang_tour test files found under tests/." >&2
    exit 1
fi

CC="${CC:-cc}"
COMMON_FLAGS=(-Wall -Wextra -Werror -std=c11 -Iinclude)
REL_FLAGS=(-O3 "${COMMON_FLAGS[@]}")
SAN_FLAGS=(-O1 -g -fsanitize=address,undefined "${COMMON_FLAGS[@]}")

total_tests=0
for src in "${test_files[@]}"; do
    base="$(basename "$src" .c)"
    extra_libs=()
    if grep -q 'pthread' "$src"; then
        extra_libs+=(-pthread)
    fi

    rel_bin="$TMP_BIN/${base}_rel"
    san_bin="$TMP_BIN/${base}_san"

    echo "=== [REL -O3] $base ==="
    extra_srcs=()
    extra_defs=()
    if [ -f "src/cjang_bridge.c" ]; then
        extra_srcs+=(src/cjang_bridge.c)
    fi
    if [ -f "src/cjang_bridge_cli.c" ] && grep -q 'cjang_bridge_cli_main' "$src"; then
        extra_srcs+=(src/cjang_bridge_cli.c)
        extra_defs+=(-DCJANG_BRIDGE_CLI_NO_MAIN)
    fi

    "$CC" "${REL_FLAGS[@]}" "${extra_defs[@]}" src/cjang_tour.c src/cjang_guard.c "${extra_srcs[@]}" "$src" \
        "${extra_libs[@]}" -o "$rel_bin"
    "$rel_bin"

    echo "=== [SAN ASan+UBSan] $base ==="
    "$CC" "${SAN_FLAGS[@]}" "${extra_defs[@]}" src/cjang_tour.c src/cjang_guard.c "${extra_srcs[@]}" "$src" \
        "${extra_libs[@]}" -o "$san_bin"
    "$san_bin"

    total_tests=$((total_tests + 1))
done

echo "[OK] Verified $total_tests test suite(s) in Release and Sanitized modes."
