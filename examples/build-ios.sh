#!/bin/bash
set -e

# Build next to this script (in examples/), regardless of the caller's cwd.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

if [ "$(uname)" != "Darwin" ]; then
    echo "Error: iOS builds require macOS."
    exit 1
fi

echo "=== Configuring iOS Xcode Project ==="
cmake -S .. -B build-ios -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=15.0 \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCMAKE_BUILD_TYPE=Debug

echo "=== iOS Project Generated! ==="
echo "You can open '$SCRIPT_DIR/build-ios/ryolib.xcodeproj' in Xcode to build, sign, and deploy the TestApp example to your iPhone."
