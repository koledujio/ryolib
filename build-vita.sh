#!/bin/bash
set -e

if [ -z "$VITASDK" ]; then
    echo "Error: VITASDK is not in your PATH."
    echo "You need to install the Vita SDK in order to build for the vita."
    echo "You can find the instructions for installing it here: https://vitasdk.org/"
    exit 1
fi

echo "=== Configuring Vita Cross-Compilation ==="
cmake -S . -B build-vita -DCMAKE_TOOLCHAIN_FILE="$VITASDK/share/vita.toolchain.cmake" -DCMAKE_BUILD_TYPE=Debug

echo "=== Building Vita App ==="
cmake --build build-vita -j 8

echo "=== Vita Build Complete! ==="
echo "You can find the vpk in 'build-vita'"