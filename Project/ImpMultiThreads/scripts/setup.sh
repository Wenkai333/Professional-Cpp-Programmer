#!/bin/bash
set -e

echo "🚀 Setting up C++ project environment..."

# Check dependencies
command -v cmake >/dev/null 2>&1 || { echo "❌ CMake is required but not installed."; exit 1; }
command -v ninja >/dev/null 2>&1 || { echo "❌ Ninja is required but not installed."; exit 1; }

# Create build directory
mkdir -p build

# Configure CMake
echo "📦 Configuring CMake..."
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Install Conan dependencies (if conanfile.txt exists)
if [ -f "conanfile.txt" ]; then
    echo "📚 Installing Conan dependencies..."
    cd build
    conan install .. --build=missing
    cd ..
fi

echo "✅ Setup complete!"
echo "💡 Run './scripts/build.sh' to build the project"
echo "🐛 Run 'code .' to open in VS Code"
