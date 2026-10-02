#!/bin/sh
set -e

# $1: default SDK (e.g. iphoneos, macosx)
# $2: action ("compile" or "link")
# $3...: arguments

DEFAULT_SDK="$1"
ACTION="$2"
SDK="${SDK_NAME:-$DEFAULT_SDK}"

if echo "$SDK" | grep -qi "simulator"; then
    FLAGS="-mios-simulator-version-min=15.0"
elif echo "$SDK" | grep -qi "iphone"; then
    FLAGS="-mios-version-min=15.0"
else
    FLAGS="-mmacosx-version-min=11.0"
fi

if [ "$ACTION" = "compile" ]; then
    INPUT_FILE="$3"
    OUTPUT_FILE="$4"
    xcrun -sdk "$SDK" metal $FLAGS -c "$INPUT_FILE" -o "$OUTPUT_FILE"
elif [ "$ACTION" = "link" ]; then
    OUTPUT_FILE="$3"
    shift 3
    xcrun -sdk "$SDK" metallib "$@" -o "$OUTPUT_FILE"
fi
