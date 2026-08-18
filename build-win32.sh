#!/bin/bash
set -e

if ! command -v cmake &> /dev/null; then
    echo "Error: 'cmake' is not installed or not in your PATH."
    exit 1
fi

if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    echo "Error: 'x86_64-w64-mingw32-gcc' is not installed."
    echo "Please install it using your package manager."
    if [ "$(uname)" == "Darwin" ]; then
        echo "If you use homebrew, you can install it with 'brew install mingw-w64'."
        exit 1
    fi
    exit 1
fi

if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "Error: 'x86_64-w64-mingw32-g++' is not installed."
    echo "Please install it using your package manager."
    if [ "$(uname)" == "Darwin" ]; then
        echo "If you use homebrew, you can install it with 'brew install mingw-w64'."
        exit 1
    fi
    exit 1
fi

echo "=== Configuring Win32 Cross-Compilation ==="
cmake -S . -B build-win32 -DCMAKE_TOOLCHAIN_FILE=mingw-toolchain.cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Debug

echo "=== Building Win32 App ==="
cmake --build build-win32 -j 8

echo "=== Win32 Build Complete! ==="
echo "You can find the executable in 'build-win32'"