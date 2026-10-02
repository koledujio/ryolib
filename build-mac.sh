#!/bin/bash
set -e

if [ "$(uname)" != "Darwin" ]; then
    echo "Error: This script is only compatible with macOS. Current OS is $(uname)."
    exit 1
fi

echo "=== Configuring macOS Build ==="
cmake -S . -B build-mac -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Debug

echo "=== Building macOS App ==="
cmake --build build-mac -j 8

echo "=== macOS Build Complete! ==="
echo "You can find the executable in 'build-mac'"