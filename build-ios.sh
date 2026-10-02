#!/bin/bash
set -e

if [ "$(uname)" != "Darwin" ]; then
    echo "Error: iOS builds require macOS."
    exit 1
fi

echo "=== Configuring iOS Xcode Project ==="
cmake -S . -B build-ios -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=15.0 \
    -DCMAKE_BUILD_TYPE=Debug

echo "=== iOS Project Generated! ==="
echo "You can open 'build-ios/GfxLibraryTest.xcodeproj' in Xcode to build, sign, and deploy to your iPhone."
