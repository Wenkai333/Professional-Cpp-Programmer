#!/bin/bash
set -e

echo "🧪 Running tests..."

# Build first
./scripts/build.sh Debug

# Run tests
cd build
ctest --output-on-failure --parallel $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "✅ All tests passed!"
