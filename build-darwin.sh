#!/bin/bash
set -e

if [ "$(uname)" != "Darwin" ]; then
    echo "Error: This script is only compatible with macOS (Darwin). Current OS is $(uname)."
    exit 1
fi

echo "=== Configuring Darwin Build ==="
cmake -S . -B build-darwin -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Debug

echo "=== Building Darwin App ==="
cmake --build build-darwin -j 8

echo "=== Darwin Build Complete! ==="
echo "You can find the executable in 'build-darwin'"