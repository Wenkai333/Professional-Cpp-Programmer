#!/bin/bash
set -e

BUILD_TYPE=${1:-Debug}
SANITIZER=${2:-none}
PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "🔨 Building project (${BUILD_TYPE})..."

# Sanitizer flags
SANITIZER_FLAGS=""
case $SANITIZER in
    thread|tsan)
        echo "🧵 Enabling ThreadSanitizer"
        SANITIZER_FLAGS="-DCMAKE_CXX_FLAGS='-fsanitize=thread -g' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=thread'"
        ;;
    address|asan)
        echo "🔍 Enabling AddressSanitizer"
        SANITIZER_FLAGS="-DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -g' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'"
        ;;
    memory|msan)
        echo "💾 Enabling MemorySanitizer"
        SANITIZER_FLAGS="-DCMAKE_CXX_FLAGS='-fsanitize=memory -g' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=memory'"
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

# Configure if needed
if [ ! -f "build/CMakeCache.txt" ]; then
    echo "📦 Configuring CMake..."
    cmake -B build -G Ninja \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        ${SANITIZER_FLAGS}
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

# Usage reminder
if [ "$SANITIZER" != "none" ]; then
    echo ""
    echo "💡 Run with sanitizer: ./build/src/main"
    if [ "$SANITIZER" = "thread" ] || [ "$SANITIZER" = "tsan" ]; then
        echo "📝 TSan tip: Run multiple times to catch race conditions"
    fi
fi
