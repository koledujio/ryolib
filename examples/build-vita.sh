#!/bin/bash
set -e

# Build next to this script (in examples/), regardless of the caller's cwd.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Fall back to the common Vita SDK install locations when VITASDK isn't exported.
if [ -z "$VITASDK" ]; then
    for candidate in /usr/local/vitasdk "$HOME/vitasdk"; do
        if [ -f "$candidate/share/vita.toolchain.cmake" ]; then
            export VITASDK="$candidate"
            break
        fi
    done
fi

if [ -z "$VITASDK" ]; then
    echo "Error: VITASDK is not set and no Vita SDK was found."
    echo "You need to install the Vita SDK in order to build for the vita."
    echo "You can find the instructions for installing it here: https://vitasdk.org/"
    exit 1
fi

echo "=== Configuring Vita Cross-Compilation ==="
cmake -S .. -B build-vita \
    -DCMAKE_TOOLCHAIN_FILE="$VITASDK/share/vita.toolchain.cmake" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCMAKE_BUILD_TYPE=Debug

echo "=== Building Vita App ==="
cmake --build build-vita -j 8

echo "=== Vita Build Complete! ==="
echo "You can find the vpk in '$SCRIPT_DIR/build-vita/examples'"