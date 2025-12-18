#!/bin/bash
set -e

BUILD_TYPE=${1:-Debug}
PARALLEL_JOBS=${2:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}

echo "🔨 Building project (${BUILD_TYPE})..."

# Configure if needed
if [ ! -f "build/CMakeCache.txt" ]; then
    echo "📦 Configuring CMake..."
    cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
fi

# Build
cmake --build build --config ${BUILD_TYPE} --parallel ${PARALLEL_JOBS}

echo "✅ Build complete!"

# Show executable locations
if [ -f "build/src/main" ]; then
    echo "🎯 Main executable: build/src/main"
fi

if [ -f "build/tests/unit_tests" ]; then
    echo "🧪 Test executable: build/tests/unit_tests"
fi
