#!/bin/bash
set -e

BUILD_TYPE=${1:-Debug}
SANITIZER=${2:-none}
PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "🔨 Building project (${BUILD_TYPE})..."

# Sanitizer flags
CXX_FLAGS=""
LINKER_FLAGS=""
case $SANITIZER in
    thread|tsan)
        echo "🧵 Enabling ThreadSanitizer"
        CXX_FLAGS="-fsanitize=thread -g"
        LINKER_FLAGS="-fsanitize=thread"
        ;;
    address|asan)
        echo "🔍 Enabling AddressSanitizer"
        CXX_FLAGS="-fsanitize=address,undefined -g"
        LINKER_FLAGS="-fsanitize=address,undefined"
        ;;
    memory|msan)
        echo "💾 Enabling MemorySanitizer"
        CXX_FLAGS="-fsanitize=memory -g"
        LINKER_FLAGS="-fsanitize=memory"
        ;;
    none)
        echo "⚡ No sanitizer"
        ;;
    *)
        echo "❌ Unknown sanitizer: $SANITIZER"
        echo "Valid options: thread, address, memory, none"
        exit 1
        ;;
esac

# Clean build if sanitizer changed
if [ -f "build/CMakeCache.txt" ] && [ "$SANITIZER" != "none" ]; then
    echo "🗑️  Cleaning build for sanitizer change..."
    rm -rf build
fi

# Configure
if [ ! -f "build/CMakeCache.txt" ]; then
    echo "📦 Configuring CMake..."
    
    if [ -n "$CXX_FLAGS" ]; then
        # Build with sanitizer
        cmake -B build -G Ninja \
            -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            -DCMAKE_CXX_FLAGS="${CXX_FLAGS}" \
            -DCMAKE_EXE_LINKER_FLAGS="${LINKER_FLAGS}"
    else
        # Build without sanitizer
        cmake -B build -G Ninja \
            -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    fi
fi

# Build
echo "🔧 Building with ${PARALLEL_JOBS} parallel jobs..."
cmake --build build --config "${BUILD_TYPE}" --parallel "${PARALLEL_JOBS}"

echo "✅ Build complete!"

# Show executable locations
if [ -f "build/src/main" ]; then
    echo "🎯 Main executable: build/src/main"
fi

if ls build/src/day* 1> /dev/null 2>&1; then
    echo "📚 Day exercises: $(ls build/src/day* | wc -l | tr -d ' ') found"
fi

if [ -f "build/tests/unit_tests" ]; then
    echo "🧪 Test executable: build/tests/unit_tests"
fi

# Usage reminder
if [ "$SANITIZER" != "none" ]; then
    echo ""
    echo "💡 Run with sanitizer: ./build/src/day2"
    if [ "$SANITIZER" = "thread" ] || [ "$SANITIZER" = "tsan" ]; then
        echo "📝 TSan tip: Run multiple times to catch race conditions"
    fi
fi