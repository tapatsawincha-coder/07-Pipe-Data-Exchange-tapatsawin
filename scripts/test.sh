#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$repo_root"

make clean >/dev/null 2>&1 || true
make >/dev/null 2>&1

output="$(./bin/pipe_lab 2>&1 || true)"

echo "$output"

# Reject unimplemented starter code
if echo "$output" | grep -q "TODO:"; then
    echo "visible test FAILED: output contains TODO placeholder text" >&2
    exit 1
fi

expected="child received: ping
parent: sent ping
child computed: 42
parent received: 42
child: got EOF after 5 bytes
parent: broken pipe detected
all tasks done"

if [[ "$output" != "$expected" ]]; then
    echo "visible test FAILED: output does not match expected" >&2
    echo "--- expected ---" >&2
    echo "$expected" >&2
    echo "--- got ---" >&2
    echo "$output" >&2
    exit 1
fi

echo "visible test: passed"